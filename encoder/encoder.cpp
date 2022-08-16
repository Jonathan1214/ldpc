#include "encoder.h"


void 
Encoder::run_encoder(const LDPC& cc, const InfoFrameType& info) {
    assert(info.size() == cc.get_info_len());
    assert(coded_output.size() == cc.get_len());
    for (size_t i = 0; i < cc.get_len(); ++i) {
        BitType cur = 0;
        for (size_t j = 0; j < cc.get_info_len(); ++j) {
            // use And operation for encoding
            cur ^= cc.get_gen_xy(i, j) & info[j];
        }
        coded_output[i] = cur;
    }
}
