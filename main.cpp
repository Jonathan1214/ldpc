#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <ctime>
#include "encoder.h"
#include "decoder.h"
#include "ldpc.h"
using namespace std;

#define rand_ra() (int)((30000. * rand()) / ((double)RAND_MAX))
#define PI 3.1415926
llr_width noise;
void AWGN(void)
{
    double random1, random2, random3, random4;
    double G, B;

    random1 = rand_ra() % 10000;
    random2 = rand_ra() % 10000;
    random3 = rand_ra() % 10000;
    random4 = rand_ra() % 10000;
    G = sqrt(-2 * log((random1 / 10000) + (random2 / 10000 / 10000) + 0.000000001));
    B = (random3 / 10000) + (random4 / 10000 / 10000);
    noise = G * cos(2 * PI * B);
}

code_configure ccf;

#define initial_code(code) do { for (auto &c : code) c = rand() % 2; } while (0);
#define configure_d(code_c, param) code_c.param = atoi(optarg);
#define configure(code_c, param) code_c.param = string(optarg);
#define configure_f(code_c, param) code_c.param = atof(optarg);
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
    };
    int o;
    while ( (o = getopt_long(argc, argv, "ehl:r:b:c:v:C:V:G:d::i::a::x:", table, NULL)) != -1) {
        switch (o) {
        case 'l':
            printf("length of code is %s\n", optarg);
            configure_d(ccf, len);
            break;
        case 'r':
            printf("row of code is %s\n", optarg);
            configure_d(ccf, row);
            break;
        case 'b':
            printf("bits of code is %s\n", optarg);
            configure_d(ccf, bits);
            break;
        case 'c':
            printf("cnfile of code is %s\n", optarg);
            configure(ccf, cnsfile);
            break;
        case 'v':
            printf("vnfile of code is %s\n", optarg);
            configure(ccf, vnsfile);
            break;
        case 'd':
            // TODO select decoding algorithm
            printf("decoding algorithm of code is %s\n", optarg);
            break;
        case 'i': 
            printf("iteraion of decoding is %s\n", optarg); 
            configure_d(ccf, iteration);
            break;
        case 'a':
            printf("attenuation of decoding is 0.%s\n", optarg);
            // a stupid bug... cannot read 0.625 from command line
            ccf.attenuation = atoi(optarg) / 1000.0;
            break;
        case 'x': 
            printf("genematrix of code is %s\n", optarg); 
            assert(optarg != NULL);
            configure(ccf, genfile);
            break;
        case 'C': 
            printf("check node weight of code is %s\n", optarg); 
            configure_d(ccf, dc);
            break;
        case 'V': 
            printf("variable node weight of code is %s\n", optarg); 
            configure_d(ccf, dv);
            break;
        case 'G': 
            printf("GF of code is %s\n", optarg); 
            configure_d(ccf, gf_1);
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

    cout << "acode" << endl;
    // ac.initial(2048, 1027, 4, 8, 256, cnf, vnf, ',');
    cout << ac.get_cns_size() << endl;
    cout << ac.get_H_size() << endl;
    cout << sizeof ac << endl;
    cout << "---------------------------" << endl;

    // channel condition set
    double ebn0 = 3.4;
    double esn0 = ac.get_code_ratio() * pow(10, ebn0 / 10.0);


    // base sequence
    info_fram_t basecode(ac.get_bits(), 0);
    // initial code
    rece_seq re_seqs(ac.get_col(), 0);
    int m = 1;
    cout << "test " << m << " code" << endl;
    int ok_sum = 0;
    time_t st = time(0);
    while (m--)
    {
        initial_code(basecode);
        if (ccf.encoding) encoder.run_encoder(ac, basecode);

        for (int i = 0; i < re_seqs.size(); ++i)
        {
            re_seqs[i] = 1 - 2 * encoder.coded_o[i];
        }

        // pass channel
        for (int i = 0; i < re_seqs.size(); ++i)
        {
            AWGN();
            re_seqs[i] = re_seqs[i] + 1.0 / sqrt(2.0 * esn0) * noise;
        }

        // run decoder
        ok_sum += decoder.run_decoder(ac, re_seqs, ebn0);
    }
    time_t ed = time(0);
    cout << "check ok code : " << ok_sum << endl;
    cout << "time used : " << ed - st << "s" << endl;
    print_any_thing(ac);
}

int main(int argc, char *argv[])
{
    srand(0); // set random seed
              //  test_decoder();
    cout << "parse start " << endl;
    parse_arg(argc, argv);

    test_decoder();
    return 0;
}
