#include <iostream>
#include "encoder.h"
#include "decoder.h"
#include "ldpc.h"

using namespace std;

LDPC ac;
Encoder encoder;

#define print_array(T, size) {\
    for (int i = 0; i < size; ++i) \
        cout << (int)T.at(i) << " "; \
    cout << endl; \
}\

void init_code() {
    string cnf("../matrix/CNsCon_mask.csv");
    string vnf("../matrix/VNsCon_mask.csv");
    string genfile("../../ldpc_gf257_multi_code_ratio/base4x8/matrix/G_mat.csv");
    cout << "acode" << endl;
    ac.initial(2048, 1027, 4, 8, 256, cnf, vnf, ',');
    cout << ac.get_cns_size() << endl;
    cout << ac.get_H_size() << endl;
    cout << sizeof ac << endl;
    cout << "len of info : " << ac.get_info_len() << endl;
    cout << "---------------------------" << endl;
    ac.set_genMat(genfile, ' ');
}


void parse_input(const string& s, InfoFrameType& out, char delim) {
    int bits = out.size();
    int ix = 0;
    int st = 0;
    int i = 0;
    for (; i < bits && ix < s.size(); ++i) {
        st = ix;
        while (ix < s.size() && s[ix] != delim) ix++;
        out[i] = stoi(s.substr(st, ix - st));
        assert(out[i] == 0 || out[i] == 1);
        ix++;
    }
    assert(i == bits);
}

bool comp_equal(InfoFrameType& case_out, InfoFrameType& coded_out) {
    assert(case_out.size() == coded_out.size());
    int size = case_out.size();
    for (int i = 0; i < size; ++i) {
        if (case_out[i] != coded_out[i]) return false;
    }
    return true;
}

void test_code() {
    ifstream fp_case_in, fp_case_out;
    string case_in_file("../../ldpc_gf257_multi_code_ratio/base4x8/matrix/case_in.csv");
    string case_out_file("../../ldpc_gf257_multi_code_ratio/base4x8/matrix/case_out.csv");
    fp_case_in.open(case_in_file);
    fp_case_out.open(case_out_file);
    assert(fp_case_in.is_open());
    string s;
    int len = ac.get_len();
    int bits = ac.get_info_len();
    char delim = ' ';
    int line = 0;
    InfoFrameType case_in(bits, 0);
    InfoFrameType case_out(len, 0);
    int ok_cnt = 0;
    while (line < ac.get_len() && getline(fp_case_in, s)) {
        parse_input(s, case_in, delim);
        if (!getline(fp_case_out, s)) exit(-1);
        parse_input(s, case_out, delim);

        encoder.run_encoder(ac, case_in);
        if (comp_equal(case_out, encoder.coded_output)) ok_cnt++;
        else {
            cout << "ok cnt : " << ok_cnt << endl;
            exit(-3);
        }

        line++;
    }
    fp_case_in.close();
    fp_case_out.close();
    cout << "input " << line << " case " << endl;
    cout << "encoder pass" << endl;
}

void test_main() {
    init_code();
    encoder.set_out_size(ac.get_len());
    test_code();
}

int main() {
    test_main();
}