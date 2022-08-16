// date: 2022/07/04
// description: encode for a ldpc code
//              Read generate matrix from file
//              Use encoder for code generate
//
// bug report contact jonathan1214@foxmail.com
//

#ifndef _ENCODER_H_
#define _ENCODER_H_
#include <iostream>
#include <vector>
#include "ldpc.h"

class Encoder {
public:
    Encoder() = default;
    Encoder(size_t size) { assert(size > 0); coded_output.resize(size); }
    inline Encoder& set_out_size(size_t size) { coded_output.resize(size); return *this; }
    void run_encoder(const LDPC& cc, const InfoFrameType& info);

    InfoFrameType coded_output; // encoder output
private:

};

#endif
