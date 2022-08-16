#include "ldpc.h"

void LDPC::initial(int _len, int _bits, int bx, int by, int _gf_1,
                   const string& _cnsfile, const string& _vnsfile, char delim,
                   char corr) {
  len_ = _len;
  bits_ = _bits;
  dc_ = by;
  dv_ = bx;
  code_ratio_ = _bits / (_len + 0.0);
  gf_1_ = _gf_1;
  row_of_h_matrix_ = bx * _gf_1;
  column_of_h_matrix_ = by * _gf_1;

  init_memo(_cnsfile, _vnsfile, delim, corr);
}

void LDPC::init_memo(const string& _cnsfile, const string& _vnsfile, char delim,
                     char corr) {
  cns_.assign(row_of_h_matrix_, NodeConn(dc_, 0));
  assert(cns_.size() == row_of_h_matrix_);
  read_conn(cns_, _cnsfile, delim, corr);

  vns_.assign(column_of_h_matrix_, NodeConn(dv_, 0));
  assert(vns_.size() == column_of_h_matrix_);
  read_conn(vns_, _vnsfile, delim, corr);
}

void LDPC::init_memo(const string& _cnsfile, const string& _vnsfile,
                     const string& _genfile, char delim, char corr) {
  cns_.assign(row_of_h_matrix_, NodeConn(dc_, 0));
  assert(cns_.size() == row_of_h_matrix_);
  read_conn(cns_, _cnsfile, delim, corr);

  vns_.assign(column_of_h_matrix_, NodeConn(dv_, 0));
  assert(vns_.size() == column_of_h_matrix_);
  read_conn(vns_, _vnsfile, delim, corr);

  genMat_.assign(column_of_h_matrix_, GeneratorMatrixColumn(bits_, 0));
  assert(genMat_.size() == column_of_h_matrix_);
  set_genMat(_genfile, ' ');
}

// read configuration from struct CodeConfigure
LDPC::LDPC(const CodeConfigure& ccf) {
  len_ = ccf.len;
  bits_ = ccf.bits;
  dc_ = ccf.dc;
  dv_ = ccf.dv;
  code_ratio_ = bits_ / (len_ + 0.0);
  gf_1_ = ccf.gf_1;
  row_of_h_matrix_ = dv_ * gf_1_;
  column_of_h_matrix_ = dc_ * gf_1_;
  if (ccf.encoding)
    init_memo(ccf.cnsfile, ccf.vnsfile, ccf.genfile, ccf.delim, ccf.corr);
  else
    init_memo(ccf.cnsfile, ccf.vnsfile, ccf.delim, ccf.corr);
}

void LDPC::read_conn(MatrixConn& to_mat, const string& file, char delim,
                     char corr) {
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
    istringstream read_str(s);
    string cur;
    for (int i = 0; i < y; ++i) {
      getline(read_str, cur, delim);
      int val = stoi(cur);
      to_mat[line][i] = val;
      to_mat[line][i] -= corr;
    }
    line++;
  }
  fp.close();
  assert(line == x);
}

void LDPC::set_genMat(const string& file, char delim) {
  ifstream fp;
  fp.open(file);
  assert(fp.is_open());
  string s;
  int line = 0;
  while (line < len_ && getline(fp, s)) {
    istringstream read_str(s);
    string cur;
    for (int i = 0; i < bits_; ++i) {
      getline(read_str, cur, delim);
      int val = stoi(cur);
      assert(val == 0 || val == 1);
      genMat_[line][i] = val;
    }
    line++;
  }
  fp.close();
  assert(line == len_);
}

// 友元函数定义
void print_any_thing(LDPC& c) {
  for (auto it : c.cns_.back()) cout << it << " ";
  cout << endl;
}
