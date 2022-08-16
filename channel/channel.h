#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <iostream>
#include <random>
#include "ldpc.h"

extern LLRWidthType noise;
#define rand_ra() (int)((30000. * rand()) / ((double)RAND_MAX))
#define PI 3.1415926

using std::normal_distribution;
using std::mt19937;

class Channel {
public:
    Channel() : nd_(0, 1) { } 
    Channel(uint_fast32_t sd, int mean, int stdvar): rng_(sd), nd_(mean, stdvar) { }
    double awgn();

private:
    mt19937 rng_;
    // normal guassian distribution
    normal_distribution<> nd_;
};


void AWGN(void);

#endif
