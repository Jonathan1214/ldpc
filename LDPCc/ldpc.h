#ifndef _LDPC_h_
#define _LDPC_h_
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

using std::vector;
using std::string;
using std::ifstream; // 读取文件
using std::ios; // 控制读取模式
using std::stoi;
using std::to_string;
using std::cout;
using std::endl;

using pos_t = int16_t;
using trans_symbol_width = float;
using bit_t = char;                           // use char to a bit
using info_fram_t = vector<bit_t>;
using llr_width = trans_symbol_width;
using rece_seq = vector<trans_symbol_width>;
using llr_seq = vector<llr_width>;
enum  de_algo {msa, spa};

// extern class encoder;
// extern class decoder;

// for code configuration
struct code_configure {
    int len;                    // code length
    int row;                    // row off H matrix
    int bits;                   // information bits length
    int dc;                     // row weight of H matrix
    int dv;                     // col weight of H matrix
    int gf_1;                   // Galoris Field minus 1
    int iteration = 50;         // iteration of decoding, default 50
    int encoding = 0;           // select encoding
    float attenuation = 0.625;  // attenuation of MSA
    string vnsfile;             // variable node connection file
    string cnsfile;             // check node connection file
    string genfile;             // generation file
    char corr = 1;              // correction of connection file
    char delim = ',';           // delim of connection file
};

class LDPC {
    friend void print_any_thing(LDPC& c);
    // friend class encoder;

public:
    using nodeConn   = vector<pos_t>;
    using matrixConn = vector<nodeConn>;
    using genMatCol_t  = info_fram_t;
    using genMat_t     = vector<genMatCol_t>;
    LDPC() = default;
    LDPC(int _len, int _bits) : len(_len), bits(_bits), code_ratio(_bits/(_len+0.0)) {}
    LDPC(const code_configure& ccf);
    // initial code
    void initial(int _len, int _bits, int bx, int by, int _gf_1, const string& _cnsfile,
                const string& _vnsfile, char delim = ',', char corr = 1);
    void init_memo(const string& _cnsfile, const string& _vnsfile, char delim, char corr);
    void init_memo(const string& _cnsfile, const string& _vnsfile, const string& _genfile, char delim, char corr);

    inline int get_len()   const { return len; }
    inline int get_info_len() const { return bits; } // !TODO
    inline bit_t get_gen_xy(int i, int j) const {
        assert(i < genMat.size() && j < genMat[0].size());
        return genMat[i][j]; };    // !TODO
    inline int get_bits()  const { return bits;    }
    inline int get_dc()    const { return dc;      }
    inline int get_dv()    const { return dv;      }
    inline int get_gf()    const { return gf_1;    }
    inline int get_row()   const { return H_row;   }
    inline int get_col()   const { return H_col;   }
    inline pos_t get_cns_xy(int x, int y) const { return cns[x][y]; }
    inline pos_t get_vns_xy(int x, int y) const { return vns[x][y]; }

    inline double get_code_ratio() const { return code_ratio; }
    inline int get_cns_size() const { return cns.size(); }
    inline string get_H_size() const { return to_string(H_row) + "x" + to_string(H_col); }
    void set_genMat(const string& file, char delim);

private:
    int len;                    // code length
    int bits;                   // messages bits length of code
    int dc;                     // weight of row of H
    int dv;                     // weight of col of H
    int gf_1;                   // Galois field
    int H_row;                  // row of H_matrix
    int H_col;                  // col of H_matrix
    double code_ratio;          // encode ratio
    matrixConn cns;             // row connections of H
    matrixConn vns;             // col connections of H
    genMat_t   genMat;          // generation matrix
    // string cns_file_path;       // cns_file_path
    // string vns_file_path;       // vns_file_path

    void read_conn(matrixConn& to_mat, const string& file, char delim, char corr);
};

#endif
