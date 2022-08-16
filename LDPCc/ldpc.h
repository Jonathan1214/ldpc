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

using PosType = int16_t;
using SymbolWidthType = float;
using BitType = char;                           // use char to a bit
using InfoFrameType = vector<BitType>;
using LLRWidthType = SymbolWidthType;
using ReceivedSequencesType = vector<SymbolWidthType>;
using LLRSequencesType = vector<LLRWidthType>;
enum  DE_ALGO {msa, spa};

// extern class encoder;
// extern class decoder;

// for code configuration
struct CodeConfigure {
    int len;                    // code length
    int row;                    // row off H matrix
    int bits;                   // information bits length
    int dc;                     // row weight of H matrix
    int dv;                     // col weight of H matrix
    int gf_1;                   // Galoris Field minus 1
    float sebn0 = 2;             // start ebn0 of simulation
    float spebn0 = 0.2;         // step ebn0 of simulation
    float eebn0 = 3;            // end ebn0 of simulation
    int iteration = 50;         // iteration of decoding, default 50
    int encoding = 0;           // select encoding
    float attenuation = 0.625;  // attenuation of MSA
    string vnsfile;             // variable node connection file
    string cnsfile;             // check node connection file
    string genfile;             // generation file
    char corr = 1;              // correction of connection file
    const char delim = ',';           // delim of connection file
};

class LDPC {
    friend void print_any_thing(LDPC& c);
    // friend class encoder;

public:
    using NodeConn   = vector<PosType>;
    using MatrixConn = vector<NodeConn>;
    using GeneratorMatrixColumn  = InfoFrameType;
    using GeneratorMatrix     = vector<GeneratorMatrixColumn>;
    LDPC() = default;
    LDPC(int _len, int _bits) : len_(_len), bits_(_bits), code_ratio_(_bits/(_len+0.0)) {}
    LDPC(const CodeConfigure& ccf);
    // initial code
    void initial(int _len, int _bits, int bx, int by, int _gf_1, const string& _cnsfile,
                const string& _vnsfile, char delim = ',', char corr = 1);
    inline void init_memo(const string& _cnsfile, const string& _vnsfile, char delim, char corr);
    inline void init_memo(const string& _cnsfile, const string& _vnsfile, const string& _genfile, char delim, char corr);

    inline int get_len()   const { return len_; }
    inline int get_info_len() const { return bits_; } // !TODO
    inline BitType get_gen_xy(int i, int j) const {
        assert(i < genMat_.size() && j < genMat_[0].size());
        return genMat_[i][j]; };    // !TODO
    inline int get_bits()  const { return bits_;    }
    inline int get_dc()    const { return dc_;      }
    inline int get_dv()    const { return dv_;      }
    inline int get_gf()    const { return gf_1_;    }
    inline int get_row()   const { return row_of_h_matrix_;   }
    inline int get_col()   const { return column_of_h_matrix_;   }
    inline PosType get_cns_xy(int x, int y) const { return cns_[x][y]; }
    inline PosType get_vns_xy(int x, int y) const { return vns_[x][y]; }

    inline double get_code_ratio() const { return code_ratio_; }
    inline int get_cns_size() const { return cns_.size(); }
    inline string get_H_size() const { return to_string(row_of_h_matrix_) + "x" + to_string(column_of_h_matrix_); }
    void set_genMat(const string& file, char delim);

private:
    int len_;                    // code length
    int bits_;                   // messages bits length of code
    int dc_;                     // weight of row of H
    int dv_;                     // weight of col of H
    int gf_1_;                   // Galois field
    int row_of_h_matrix_;                  // row of H_matrix
    int column_of_h_matrix_;                  // col of H_matrix
    LLRWidthType code_ratio_;          // encode ratio
    MatrixConn cns_;             // row connections of H
    MatrixConn vns_;             // col connections of H
    GeneratorMatrix   genMat_;          // generation matrix
    // string cns_file_path;       // cns_file_path
    // string vns_file_path;       // vns_file_path

    void read_conn(MatrixConn& to_mat, const string& file, char delim, char corr);
};

#endif
