#include "encoder.h"


void 
Encoder::run_encoder(const LDPC& cc, const info_fram_t& info) {
    assert(info.size() == cc.get_info_len());
    assert(coded_o.size() == cc.get_len());
    for (size_t i = 0; i < cc.get_len(); ++i) {
        bit_pos_t cur = 0;
        for (size_t j = 0; j < cc.get_info_len(); ++j) {
            cur ^= cc.get_gen_xy(i, j) ^ info[j];
        }
        coded_o[i] = cur;
    }
}
