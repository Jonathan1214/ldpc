#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <iostream>
#include <random>

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


#endif
