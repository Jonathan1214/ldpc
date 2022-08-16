#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <ctime>
#include "encoder.h"
#include "decoder.h"
#include "ldpc.h"
#include "channel.h"

using namespace std;

time_t curtime;
tm* logtime;

CodeConfigure ccf;
const int kMaxBlock = 1e6;
#define LOG_DE(ebn0, tot_blk, err_blk, err_bits, type) do { \
        curtime = time(0); \
        logtime = localtime(&curtime); \
        fprintf(stdout, type" [%02d-%02d-%02d %02d:%02d:%02d]  %6.2f  %10llu  %4d  %5d\n", logtime->tm_year + 1900, logtime->tm_mon + 1, logtime->tm_mday, \
                                logtime->tm_hour, logtime->tm_min, logtime->tm_sec, \
                                ebn0, tot_blk, err_blk, err_bits); \
        fflush(stdout); \
    } while (0)\

#define LOG_DE_RATIO(ebn0, tot_blk, err_blk, err_bits, frame_len, type) do { \
        curtime = time(0); \
        logtime = localtime(&curtime); \
        fprintf(stdout, type" [%02d-%02d-%02d %02d:%02d:%02d]  %6.2f  %10llu  %4d  %5d  %.10f  %.10f\n", logtime->tm_year + 1900, logtime->tm_mon + 1, logtime->tm_mday, \
                                logtime->tm_hour, logtime->tm_min, logtime->tm_sec, \
                                ebn0, tot_blk, err_blk, err_bits, (double)err_blk/tot_blk, (double)err_bits/tot_blk/frame_len); \
        fflush(stdout); \
    } while (0)\

#define INITIAL_CODE(code) do { for (auto &c : code) c = rand() % 2; } while (0)
#define CONFIGURE_D(code_c, param) code_c.param = atoi(optarg);
#define CONFIGURE(code_c, param) code_c.param = string(optarg);
#define CONFIGURE_F(code_c, param) code_c.param = atof(optarg);
#define HELP() cout << "This is a QC-LDPC simulator which show how well a code is\n"                       << endl; \
                cout << "Here is the configuration of the simulator:"                                       << endl; \
                cout << " --len             code length                     ex: 2048"                       << endl; \
                cout << " --row             row of H matrix                 ex: 1024"                       << endl; \
                cout << " --bits            info bits length                ex: 1027"                       << endl; \
                cout << " --cnfile          row connection file             ex: ./matrix/CnsCon.csv"        << endl; \
                cout << " --vnfile          col connection file             ex: ./matrix/VnsCon.csv"        << endl; \
                cout << " --cnweight        check node weght of code	       ex: 8"                          << endl; \
                cout << " --vnweight        variable node weight of code    ex: 4"                          << endl; \
                cout << " --gfield          Galois Fiels size minus 1       ex: 256"                        << endl; \
                cout << " --sebno           start ebn0 of simulation        ex: 2.5"                        << endl; \
                cout << " --stepebno        step ebn0 of simulation         ex: 0.2"                        << endl; \
                cout << " --eebno           end  ebn0 of simulation         ex: 3.0"                        << endl; \
                cout << " [--file]          read configuration from file    ex: config !!!!!!!!! NOT USED"  << endl; \
                cout << " [--decoding]      decoding selection              default or 0->msa 1->spa"       << endl; \
                cout << " [--iteration]     iteration of decoding           ex: 50"                         << endl; \
                cout << " [--attenuation]   attenuation of msa 	           ex: 0625 (due to a bug in command line input, only integer input is OK)"          << endl; \
                cout << " [--encoding]      select encoding		           ex:"                            << endl; \
                cout << " [--genematrix]    generation matrix file          ex: ./matrix/G.csv"             << endl; \
                cout << " --help            get help information"                                           << endl; \
                cout << endl; \
                cout << "Also, you can use short args"                                                      << endl; \
                cout << " -l                code length                     ex: 2048"                       << endl; \
                cout << " -r                row of H matrix                 ex: 1024"                       << endl; \
                cout << " -b                info bits length                ex: 1027"                       << endl; \
                cout << " -c                row connection file	           ex: ./matrix/CnsCon.csv"        << endl; \
                cout << " -v                col connection file             ex: ./matrix/VnsCon.csv"        << endl; \
                cout << " -C                check node weght of code	       ex: 8"                          << endl; \
                cout << " -V                variable node weight of code    ex: 4"                          << endl; \
                cout << " -G                Galois Fiels size minus 1       ex: 25"                         << endl; \
                cout << " -s                start ebn0 of simulation        ex: 2.5"                        << endl; \
                cout << " -S                step ebn0 of simulation         ex: 0.2"                        << endl; \
                cout << " -E                end  ebn0 of simulation         ex: 3.0"                        << endl; \
                cout << " [-d]              decoding selection              default or 0->msa 1->spa"       << endl; \
                cout << " [-i]              iteration of decoding           ex: 50"                         << endl; \
                cout << " [-a]              attenuation of msa 	           ex: 625 (due to a bug in command line input, only integer input is OK)"                      << endl; \
                cout << " [-f]              read configuration from file    ex: config !!!!!!!! NOT USE"    << endl; \
                cout << " [-e]              select encoding		           ex:"                            << endl; \
                cout << " [-x]              generation matrix file          ex: ./matrix/G.csv"             << endl; \
                cout << " -h                get help information"                                           << endl; \

static inline void parse_arg(int argc, char *argv[]) {
    const struct option table[] = {
        {"len" 		   , required_argument, NULL, 'l'},
        {"row"		   , required_argument, NULL, 'r'},
        {"bits" 	   , required_argument, NULL, 'b'},
        {"cnfile"      , required_argument, NULL, 'c'},
        {"vnfile"	   , required_argument, NULL, 'v'},
        {"decoding"    , optional_argument, NULL, 'd'},
        {"iteration"   , optional_argument, NULL, 'i'},
        {"attenuation" , optional_argument, NULL, 'a'},
        {"genematrix"  , required_argument, NULL, 'x'},
        {"encoding"    , no_argument      , NULL, 'e'},
        {"help"        , no_argument      , NULL, 'h'},
        {"cnweight"    , required_argument, NULL, 'C'},
        {"vnweight"    , required_argument, NULL, 'V'},
        {"gfield"      , required_argument, NULL, 'G'},
        {"sebno"       , required_argument, NULL, 's'},
        {"stepebno"    , required_argument, NULL, 'S'},
        {"eebno"       , required_argument, NULL, 'E'},
    }; 
    int o;
    while ( (o = getopt_long(argc, argv, "ehl:r:b:c:v:C:V:G:d::i::a::x:s:S:E:", table, NULL)) != -1) {
        switch (o) {
        case 'l':
            printf("length of code is %s\n", optarg);
            CONFIGURE_D(ccf, len);
            break;
        case 'r':
            printf("row of code is %s\n", optarg);
            CONFIGURE_D(ccf, row);
            break;
        case 'b':
            printf("bits of code is %s\n", optarg);
            CONFIGURE_D(ccf, bits);
            break;
        case 'c':
            printf("cnfile of code is %s\n", optarg);
            CONFIGURE(ccf, cnsfile);
            break;
        case 'v':
            printf("vnfile of code is %s\n", optarg);
            CONFIGURE(ccf, vnsfile);
            break;
        case 'd':
            // TODO select decoding algorithm
            printf("decoding algorithm of code is %s\n", optarg);
            break;
        case 'i': 
            printf("iteraion of decoding is %s\n", optarg); 
            CONFIGURE_D(ccf, iteration);
            break;
        case 'a':
            printf("attenuation of decoding is 0.%s\n", optarg);
            // a stupid bug... cannot read 0.625 from command line
            ccf.attenuation = atoi(optarg) / 1000.0;
            break;
        case 'x': 
            printf("genematrix of code is %s\n", optarg); 
            assert(optarg != NULL);
            CONFIGURE(ccf, genfile);
            break;
        case 'C': 
            printf("check node weight of code is %s\n", optarg); 
            CONFIGURE_D(ccf, dc);
            break;
        case 'V': 
            printf("variable node weight of code is %s\n", optarg); 
            CONFIGURE_D(ccf, dv);
            break;
        case 'G': 
            printf("GF of code is %s\n", optarg); 
            CONFIGURE_D(ccf, gf_1);
            break;
        case 's': 
            printf("start ebno of simulation is %s\n", optarg); 
            CONFIGURE_F(ccf, sebn0);
            break;
        case 'S': 
            printf("step ebno of simulation is %s\n", optarg); 
            CONFIGURE_F(ccf, spebn0);
            break;
        case 'E': 
            printf("end ebno of simulation %s\n", optarg); 
            CONFIGURE_F(ccf, eebn0);
            break;
        case 'e': 
            printf("encoding selection is %s\n", optarg); 
            // select encoding
            ccf.encoding = 1;
            break;
        case 'h': 
            HELP();
            exit(0);
            break;
        default:
            printf("error usage\n");
            exit(-1);
        }
    }
    if (ccf.encoding && ccf.genfile.size() == 0) exit(-3);
    return ;
}


void test_decoder()
{
    LDPC ac(ccf);
    // construct a decoder and encoder
    Decoder decoder(ccf);
    Encoder encoder(ccf.len);
    // initial Channel
    Channel ch;

    cout << "acode" << endl;
    // ac.initial(2048, 1027, 4, 8, 256, cnf, vnf, ',');
    cout << ac.get_cns_size() << endl;
    cout << ac.get_H_size() << endl;
    cout << sizeof ac << endl;
    fprintf(stdout, "----------------\n[START]\n");

    // channel condition set


    // base sequence
    InfoFrameType basecode(ac.get_bits(), 0);
    // initial code
    ReceivedSequencesType re_seqs(ac.get_col(), 0);
    time_t st = time(0);
    for (float ebn0 = ccf.sebn0; ebn0 < ccf.eebn0 + 0.0001; ebn0 += ccf.spebn0) {
        double esn0 = ac.get_code_ratio() * pow(10, ebn0 / 10.0);
        unsigned long long tot_blk = 0;
        int err_blk = 0;
        int err_bits = 0;
        while (err_blk < 10)
        {
            INITIAL_CODE(basecode);
            if (ccf.encoding) encoder.run_encoder(ac, basecode);

            for (int i = 0; i < re_seqs.size(); ++i)
            {
                re_seqs[i] = 1 - 2 * encoder.coded_output[i];
            }

            // pass channel
            for (int i = 0; i < re_seqs.size(); ++i)
            {
                // AWGN();
                re_seqs[i] = re_seqs[i] + 1.0 / sqrt(2.0 * esn0) * ch.awgn(); // noise;
            }

            // run decoder
            tot_blk++;
            if (decoder.run_decoder(ac, re_seqs, ebn0)) {
                err_blk++;
                err_bits += decoder.get_err_bits(encoder.coded_output);
            }
            if (tot_blk % kMaxBlock == 0)
                LOG_DE_RATIO(ebn0, tot_blk, err_blk, err_bits, ccf.len, "[ROUTINE]");
        }
        LOG_DE_RATIO(ebn0, tot_blk, err_blk, err_bits, ccf.len, "[OK]");
    }
    fprintf(stdout, "[END]----------------\n\n");
}

int main(int argc, char *argv[])
{
    srand(0); // set random seed
    // srand((unsigned)time(NULL));
    //  test_decoder();
    fprintf(stdout, "\n\n");
    parse_arg(argc, argv);
    test_decoder();
    return 0;
}
