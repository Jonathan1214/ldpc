#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <iostream>
#include <random>
#include "ldpc.h"

extern llr_width noise;
#define rand_ra() (int)((30000. * rand()) / ((double)RAND_MAX))
#define PI 3.1415926

using std::normal_distribution;
using std::mt19937;

class Channel {
public:
    Channel() : nd(0, 1) { } 
    Channel(uint_fast32_t sd, int mean, int stdvar): rng(sd), nd(mean, stdvar) { }
    double awgn();

private:
    mt19937 rng;
    // normal guassian distribution
    normal_distribution<> nd;
};


void AWGN(void);

#endif
