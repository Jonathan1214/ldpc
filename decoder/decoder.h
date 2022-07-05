#ifndef _DECODER_H_
#define _DECODER_H_
#include "../LDPCc/ldpc.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstring>

using std::swap;
using std::min_element;
using std::distance;


class Decoder {
public:

    Decoder() = default;
    Decoder(int _iteration, double _attenuation) : iteration(_iteration), 
            attenuation(_attenuation) {}

    // use MSA by default
    int run_decoder(const LDPC &cc, const rece_seq &sq, double ebn0, de_algo algo = msa);

    Decoder &set_iter(int iter) {
        this->iteration = iter;
        return *this;
    }

    Decoder &set_atten(double atten) {
        this->attenuation = atten;
        return *this;
    }

    Decoder &set_condition(int iter, double atten) {
        return this->set_iter(iter).set_atten(atten);
    }

    const vector<bit_t>& get_out_seqs() const { return out_seqs; }
    int get_iter() const { return iteration; }
    double get_atten() const { return attenuation; }

private:
    int run_use_msa(const LDPC &cc, const rece_seq &sq, double ebn0);
    int run_use_spa(const LDPC &cc, const rece_seq &sq, double ebn0);

    void get_llr(const rece_seq &sq, double code_ratio, double ebn0);
    void cn_update(const LDPC &cc);
    void vn_update(const LDPC &cc);
    int check(const LDPC &cc); // return checksum of all row check of LDPC code

    int iteration;              // iteration for LDPC decoding
    double attenuation;         // attenuation
    vector<llr_seq> cns_llr;    // llr during vn update
    vector<llr_seq> vns_llr;    // llr during vn update
    llr_seq sum_llr;            // sum of all llr during vn update
    llr_seq initial_llr;        // channel info
    vector<bit_t> out_seqs;   // decoder output

    unsigned long long checkfailed = 0;
};


#endif
