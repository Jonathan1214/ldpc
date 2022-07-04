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
#include "../LDPCc/ldpc.h"

class Encoder {
public:
    Encoder() = default;
    void run_encoder(const LDPC& cc, const info_fram_t& info);

    info_fram_t coded_o; // encoder output
private:

};

#endif
