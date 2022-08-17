#include <getopt.h>
#include <unistd.h>

#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "channel.h"
#include "decoder.h"
#include "encoder.h"
#include "ldpc.h"

using std::cerr;
using std::ofstream;
using MatrixConn = LDPC::MatrixConn;
using GeneratorMatrix = LDPC::GeneratorMatrix;
using GeneratorMatrixColumn = LDPC::GeneratorMatrixColumn;

const int blk_size = 1024;
enum GENERATOR_MATRIX_IDX {
  size_4x8_start  = 0,
  size_4x8_end    = 1024,
  size_4x12_start = 1024,
  size_4x12_end   = 2048,
  size_4x16_start = 2048,
  size_4x16_end   = 3072,
  size_4x20_start = 3072,
  size_4x20_end   = 4096,
  size_4x24_start = 4096,
  size_4x24_end   = 5120
};

struct BlockGeneratorMatrix {
  GeneratorMatrix blk1 =
      GeneratorMatrix(blk_size, GeneratorMatrixColumn(blk_size));
  GeneratorMatrix blk2 =
      GeneratorMatrix(blk_size, GeneratorMatrixColumn(blk_size));
  GeneratorMatrix blk3 =
      GeneratorMatrix(blk_size, GeneratorMatrixColumn(blk_size));
  GeneratorMatrix blk4 =
      GeneratorMatrix(blk_size, GeneratorMatrixColumn(blk_size));
  GeneratorMatrix blk5 =
      GeneratorMatrix(blk_size, GeneratorMatrixColumn(blk_size));
} blk_gene_mat;

void read_a_block(GeneratorMatrix& mat, const string& filename, char delim) {
  int n = mat.size();
  int m = mat[0].size();
  ifstream fp;
  fp.open(filename);
  assert(fp.is_open());
  string line;
  int nline = 0;
  while (getline(fp, line)) {
    istringstream read_str(line);
    string cur;
    for (int i = 0; i < m; ++i) {
      getline(read_str, cur, delim);
      int val = stoi(cur);
      mat[nline][i] = val;
    }
    nline++;
  }
  fp.close();
}

// based on the order of memory of generator code
class BlockCode {
  // typedef GeneratorMatrix::iterator GeneratorMatrixIter;
  using GeneratorMatrixIter = GeneratorMatrix::iterator;

 public:
  BlockCode(int blksize, int blkcnt)
      : blkall_(blksize, GeneratorMatrixColumn(blkcnt * blksize)) {}
  // void encoder_a_block(InfoFrameType& out_sequence, const InforFramType&
  // in_sequence, const GeneratorMatrixIter first, const GeneratorMatrixIter
  // last) {
  //   int expect_out_len = std::distance(first, last);
  //   int expect_bits_len = std::distance(first, last);
  //   int in_len = in_sequence.size();
  //   int out_len = out_sequence.size();
  //   assert()
  // }
  // 初始化生成矩阵
  BlockCode& init_generator_matrix(const string& filename, char delim) {
    ::read_a_block(blkall_, filename, delim);
    return *this;
  }

  // 测试一个 block 编码
  void encocde_a_block(InfoFrameType& out_sequence,
                       const InfoFrameType& in_sequence, int first,
                       int last) const {
    int expect_info_len = last - first;
    int in_len = in_sequence.size();
    int out_len = out_sequence.size();
    assert(in_len == expect_info_len);
    assert(last <= blkall_[0].size());
    assert(out_len == blkall_.size());

    // based on the memory order of blkall_
    for (int i = 0; i < out_len; ++i) {
      BitType cur = 0;
      for (int j = first; j < last; ++j) cur ^= blkall_[i][j] & in_sequence[j];
      out_sequence[i] = cur;
    }
  }

 private:
  GeneratorMatrix blkall_;
  // GeneratorMatrixIter start_p_;
  // GeneratorMatrixIter end_p_;
};

GeneratorMatrix blkall(blk_size, GeneratorMatrixColumn(5 * blk_size));

CodeConfigure ccf4x24 = {
    .len = 6144,      //  code length
    .row = 1024,      //  row off H matrix};
    .bits = 5123,     //  information bits length
    .dc = 24,         //  row weight of H matrixCodeConfigure ccf4x12 =
    .dv = 4,          //  col weight of H matrix
    .gf_1 = 256,      // Galoris Field minus 1};
    .sebn0 = 2.5,     //  start ebn0 of simulation
    .spebn0 = 0.2,    //  step ebn0 of simulationint main() {
    .eebn0 = 3.5,     //  end ebn0 of simulation  return 0;
    .iteration = 50,  //   iteration of decoding, default 50}
    .attenuation = 0.75,
    .vnsfile = "./matrix/VNsCon257_4x24.csv",
    .cnsfile = "./matrix/CNsCon257_4x24.csv",
};

CodeConfigure ccf4x12 = {
    .len = 3072,      //  code length
    .row = 1024,      //  row off H matrix};
    .bits = 2051,     //  information bits length
    .dc = 12,         //  row weight of H matrixCodeConfigure ccf4x12 =
    .dv = 4,          //  col weight of H matrix
    .gf_1 = 256,      // Galoris Field minus 1};
    .sebn0 = 2.5,     //  start ebn0 of simulation
    .spebn0 = 0.2,    //  step ebn0 of simulationint main() {
    .eebn0 = 3.5,     //  end ebn0 of simulation  return 0;
    .iteration = 50,  //   iteration of decoding, default 50}
    .attenuation = 0.75,
    .vnsfile = "./matrix/VNsCon257_4x12.csv",
    .cnsfile = "./matrix/CNsCon257_4x12.csv",
};

CodeConfigure ccf4x8 = {
    .len = 2048,      //  code length
    .row = 1024,      //  row off H matrix};
    .bits = 1027,     //  information bits length
    .dc = 8,          //  row weight of H matrixCodeConfigure ccf4x12 =
    .dv = 4,          //  col weight of H matrix
    .gf_1 = 256,      // Galoris Field minus 1};
    .sebn0 = 1.9,     //  start ebn0 of simulation
    .spebn0 = 0.2,    //  step ebn0 of simulationint main() {
    .eebn0 = 2.3,     //  end ebn0 of simulation  return 0;
    .iteration = 50,  //   iteration of decoding, default 50}
    .encoding = 1,    // enable encoding
    .attenuation = 0.75,
    .vnsfile = "./matrix/VNsCon257_4x12.csv",
    .cnsfile = "./matrix/CNsCon257_4x12.csv",
    .genfile = "./matrix/G_mat.csv"};
void read_conn(MatrixConn& to_mat, const string& file, char delim, char corr) {
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

// test_enable a blk
void encode_a_block(InfoFrameType& out_sequence,
                    const GeneratorMatrix& gene_mat,
                    InfoFrameType& in_sequence) {
  int n = gene_mat.size();
  int m = gene_mat[0].size();
  assert(in_sequence.size() == m);
  assert(out_sequence.size() == n);
  for (int i = 0; i < n; ++i) {
    BitType cur = 0;
    for (int j = 0; j < m; ++j) {
      // use And operation for encoding
      cur ^= gene_mat[i][j] & in_sequence[j];
    }
    out_sequence[i] = cur;
  }
  cerr << "encode done." << endl;
}

void encode_a_block_colwise(InfoFrameType& out_sequence,
                            const GeneratorMatrix& gene_mat,
                            InfoFrameType& in_sequence) {
  int n = gene_mat.size();
  int m = gene_mat[0].size();
  assert(in_sequence.size() == m);
  assert(out_sequence.size() == n);
  for (int i = 0; i < n; ++i) {
    BitType cur = 0;
    for (int j = 0; j < m; ++j) {
      // use And operation for encoding
      cur ^= gene_mat[i][j] & in_sequence[j];
    }
    out_sequence[i] = cur;
  }
  cerr << "encode done." << endl;
}

namespace test {

// 打印输出到文件
inline void out_to_file(const InfoFrameType& seq, const string& fname) {
  ofstream fp;
  fp.open(fname);
  assert(fp.is_open());
  int n = seq.size();
  for (int i = 0; i < n; ++i) fp << seq[i] - 0 << " ";
  fp.close();
}

void test_all_ones_encode(const GeneratorMatrix& mat, const string& fname) {
  int n = mat.size();
  int m = mat[0].size();
  InfoFrameType in_seq(m);
  InfoFrameType out_seq(n);
  for (int i = 0; i < m; ++i) in_seq[i] = 1;

  // encoder
  encode_a_block(out_seq, mat, in_seq);
  ofstream fp;
  fp.open(fname);
  assert(fp.is_open());
  for (int i = 0; i < n; ++i) fp << out_seq[i] - 0 << " ";
  fp.close();
}

// 从 BlockCode 进行测试
void test_all_ones_blk_code(const BlockCode& cc, const string& fname,
                            int blksize, int first,
                            int last) {
  int m = last - first;
  InfoFrameType in_seq(m);
  InfoFrameType out_seq(blksize);
  for (int i = 0; i < m; ++i) in_seq[i] = 1;
  cc.encocde_a_block(out_seq, in_seq, first, last);
  out_to_file(out_seq, fname);
}

}  // namespace test

namespace demo {}  // namespace demo

int main() {
  cout << "This a multi-ratio test fucntion" << endl;
  cout << ccf4x12.vnsfile << endl;
  GeneratorMatrix mat(ccf4x8.len, GeneratorMatrixColumn(ccf4x8.bits));
  cout << ccf4x8.genfile << endl;
  read_a_block(mat, ccf4x8.genfile, ' ');
  cout << "read ok" << endl;
  // test blk
  cout << sizeof(blk_gene_mat) << " " << sizeof(blk_gene_mat.blk1) << endl;
  read_a_block(blk_gene_mat.blk1, "./matrix/partGFile1.csv", ',');
  read_a_block(blk_gene_mat.blk2, "./matrix/partGFile2.csv", ',');
  read_a_block(blk_gene_mat.blk3, "./matrix/partGFile3.csv", ',');
  read_a_block(blk_gene_mat.blk4, "./matrix/partGFile4.csv", ',');
  read_a_block(blk_gene_mat.blk5, "./matrix/partGFile5.csv", ',');
  cout << "read ok" << endl;
  read_a_block(blkall, "./matrix/partG.csv", ',');
  cout << "read ok" << endl;
  ;

  test::test_all_ones_encode(blk_gene_mat.blk1,
                             "./test-output-logs/blk1_test.txt");
  test::test_all_ones_encode(blk_gene_mat.blk2,
                             "./test-output-logs/blk2_test.txt");
  test::test_all_ones_encode(blk_gene_mat.blk3,
                             "./test-output-logs/blk3_test.txt");
  test::test_all_ones_encode(blk_gene_mat.blk4,
                             "./test-output-logs/blk4_test.txt");
  test::test_all_ones_encode(blk_gene_mat.blk5,
                             "./test-output-logs/blk5_test.txt");
  cout << sizeof(blkall) << " " << blkall.size() << endl;
  test::test_all_ones_encode(blkall, "./test-output-logs/blkall_test_ones.txt");

  BlockCode cc(1024, 5);
  cc.init_generator_matrix("./matrix/partG.csv", ',');

  test::test_all_ones_blk_code(cc, "./test-output-logs/blk_ones1.txt", 1024,
                               GENERATOR_MATRIX_IDX::size_4x8_start,
                               GENERATOR_MATRIX_IDX::size_4x8_end);
  // test::test_all_ones_blk_code(cc, "./test-output-logs/blk_ones2.txt", 1024, 5,
  //                              2);

  // encode example
  // LDPC cc(ccf4x24);
  Decoder decoder;

  return 0;
}