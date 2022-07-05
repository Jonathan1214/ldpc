#include "ldpc.h"

void LDPC::initial(int _len, int _bits, int bx, int by, int _gf_1, string _cnsfile,
                string _vnsfile, char delim, char corr) {

    len = _len;
    bits = _bits;
    dc = by;
    dv = bx;
    code_ratio = _bits / (_len + 0.0);

    gf_1 = _gf_1;
    H_row = bx*_gf_1;
    H_col = by*_gf_1;

    cns.assign(H_row, nodeConn(by, 0));
    vns.assign(H_col, nodeConn(bx, 0));
    genMat.assign(H_col, genMatCol_t(bits, 0));

    assert(cns.size() == H_row);
    assert(vns.size() == H_col);
    assert(genMat.size() == H_col);

    read_conn(cns, _cnsfile, delim, corr);
    read_conn(vns, _vnsfile, delim, corr);

}

void LDPC::read_conn(matrixConn& to_mat, string& file, char delim, char corr) {
    int x = to_mat.size();
    assert(x > 0);
    int y = to_mat[0].size();
    ifstream fp;
    fp.open(file);
    assert(fp.is_open());
    string s;
    int line = 0;
    while (line < x && getline(fp, s)) {
        // 分隔符可能是 ',' 或者 ' '
        int ix = 0;
        int st = ix;
        for (int i = 0; i < y; ++i) {
            st = ix;
            while (ix < s.size() && s[ix] != delim) ix++;
            to_mat[line][i] = stoi(s.substr(st, ix - st));
            assert(to_mat[line][i] + 1 >= corr);
            to_mat[line][i] -= corr;
            ix++;
        }
        line++;
    }
    fp.close();
    assert(line == x);
}

void
LDPC::set_genMat(const string& file, char delim) {
    ifstream fp;
    fp.open(file);
    assert(fp.is_open());
    string s;
    int line = 0;
    while (line < len && getline(fp, s)) {
       int ix = 0;
       int st = 0;
       for (int i = 0; i < bits; ++i) {
           st = ix;
           while (ix < s.size() && s[ix] != delim) ix++;
           genMat[line][i] = stoi(s.substr(st, ix - st));
           assert(genMat[line][i] == 0 || genMat[line][i] == 1);
           ix++;
       }
       line++;
    }
    fp.close();
    assert(line == len);
}

// 友元函数定义
void print_any_thing(LDPC &c) {
    for (auto it : c.cns.back()) 
        cout << it << " ";
    cout << endl;
}

