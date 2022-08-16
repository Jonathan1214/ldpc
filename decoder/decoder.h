#ifndef _DECODER_H_
#define _DECODER_H_
#include "ldpc.h"
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
    Decoder(int _iteration, float _attenuation) : iteration(_iteration), 
            attenuation(_attenuation) {}
    Decoder(const CodeConfigure& ccf);
    void initial(const LDPC& cc);
    void initial(const CodeConfigure& ccf);
    // use MSA by default
    int run_decoder(const LDPC &cc, const ReceivedSequencesType &sq, double ebn0, DE_ALGO algo = msa);

    inline Decoder &set_iter(int iter) {
        this->iteration = iter;
        return *this;
    }

    inline Decoder &set_atten(double atten) {
        this->attenuation = atten;
        return *this;
    }

    inline Decoder &set_condition(int iter, double atten) {
        return this->set_iter(iter).set_atten(atten);
    }

    const InfoFrameType& get_out_seqs() const { return decoded_sequence_; }
    inline int get_iter() const { return iteration; }
    inline float get_atten() const { return attenuation; }

    int get_err_bits(const InfoFrameType& base) const;

private:
    int run_use_msa(const LDPC &cc, const ReceivedSequencesType &sq, double ebn0);
    int run_use_spa(const LDPC &cc, const ReceivedSequencesType &sq, double ebn0);

    inline void get_llr(const ReceivedSequencesType &sq, double code_ratio, double ebn0);
    void cn_update(const LDPC &cc);
    void vn_update(const LDPC &cc);
    inline int check(const LDPC &cc); // return checksum of all row check of LDPC code

    int iteration;              // iteration for LDPC decoding
    float attenuation;         // attenuation
    vector<LLRSequencesType> cns_llr_;    // llr during vn update
    vector<LLRSequencesType> vns_llr_;    // llr during vn update
    LLRSequencesType sum_llr_;            // sum of all llr during vn update
    LLRSequencesType initial_llr_;        // channel info
    InfoFrameType decoded_sequence_;     // decoder output

    unsigned long long checkfailed = 0;
};

#endif
