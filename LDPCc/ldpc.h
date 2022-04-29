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

// extern class encoder;
// extern class decoder;

class LDPC {
    friend void print_any_thing(LDPC& c);
    // friend class encoder;

public:
    using pos = uint16_t ;
    using nodeConn = vector<pos>;
    using matrixConn = vector<nodeConn>;
    LDPC() = default;
    LDPC(int _len, int _bits) : len(_len), bits(_bits), code_ratio(_bits/(_len+0.0)) {}

    // initial code
    void initial(int _len, int _bits, int bx, int by, int gf, string _cnsfile,
                string _vnsfile, char delim = ' ');

    int get_len()   const { return len;     }
    int get_bits()  const { return bits;    }
    int get_dc()    const { return dc;      }
    int get_dv()    const { return dv;      }
    int get_gf()    const { return gf_1;    }
    int get_row()   const { return H_row;   }
    int get_col()   const { return H_col;   }
    pos get_cns_xy(int x, int y) const { return cns[x][y]; }
    pos get_vns_xy(int x, int y) const { return vns[x][y]; }
    
    double get_code_ratio() const { return code_ratio; }
    int get_cns_size() const { return cns.size(); }
    string get_H_size() const { return to_string(H_row) + "x" + to_string(H_col); }

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
    // string cns_file_path;       // cns_file_path
    // string vns_file_path;       // vns_file_path

    void read_conn(matrixConn& to_mat, string& file, char delim);
};

#endif
