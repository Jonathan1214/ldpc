#include <iostream>
#include <direct.h>
#include <ctime>
#include "configure.h"
#include "encoder.h"
#include "decoder.h"
#include "ldpc.h"
using namespace std;


#define rand_ra()           (int)((30000.*rand())/((double)RAND_MAX))
#define PI                  3.1415926
llr_width noise;
void AWGN(void)
{
    double  random1, random2, random3, random4;
    double  G, B;

    random1 = rand_ra()%10000;	random2 =  rand_ra()%10000;
    random3 = rand_ra()%10000;	random4 = rand_ra()%10000;
    G = sqrt(-2*log((random1/10000) + (random2/10000/10000) + 0.000000001));
    B = (random3/10000) + (random4/10000/10000);
    noise = G * cos(2*PI*B);
}

namespace test_msa
{
    
} // namespace test_msa


int main(int, char**) {
    srand((unsigned)time(NULL));    // set random seed
    std::cout << "Hello, world!\n";
    std::cout << "good\n";

    string cnf("../matrix/CNsCon_mask.csv");
    string vnf("../matrix/VNsCon_mask.csv");
    LDPC ac;
    cout << "acode" << endl;
    ac.initial(2048, 1024, 4, 8, 256, cnf, vnf, ',');
    cout << ac.get_cns_size() << endl;
    cout << ac.get_H_size() << endl;
    cout << sizeof ac << endl;
    cout << "---------------------------" << endl;

    // channel condition set
    double ebn0 = 2.3;
    double esn0 = ac.get_code_ratio() * pow(10, ebn0/10.0);

    // construct a decoder
    Decoder decoder(50, 0.625);

    // base sequence
    vector<char> basecode(ac.get_col(), 0);
    rece_seq re_seqs(ac.get_col(), 1);
    int m = 10000;
    cout << "test " << m << " code" << endl;
    int ok_sum = 0;
while (m--) {
    for (int i = 0; i < basecode.size(); ++i) {
        re_seqs[i] = 1 - 2*basecode[i];
    }

    // pass channel
    for (int i = 0; i < re_seqs.size(); ++i) {
        AWGN();
        re_seqs[i] = re_seqs[i] + 1.0 / sqrt(2.0*esn0) * noise;
    }


    // run decoder
    ok_sum += decoder.run_decoder(ac, re_seqs, ebn0);
}

cout << "check ok code : " << ok_sum << endl;

    return 0;
}
