#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <iostream>
#include <random>

#include "ldpc.h"

extern LLRWidthType noise;
#define rand_ra() (int)((30000. * rand()) / ((double)RAND_MAX))
#define PI 3.1415926

using std::mt19937;
using std::normal_distribution;

class Channel {
 public:
  Channel() : nd_(0, 1) {}
  Channel(uint_fast32_t sd, int mean, int stdvar)
      : rng_(sd), nd_(mean, stdvar) {}
  double awgn();

  /**@brief 设置新的随机数种子
   * @param[in] 
   *
  */
  inline void reset_channel() {
    rng_.seed(std::random_device{}());
  }

 private:
  mt19937 rng_;
  // normal guassian distribution
  normal_distribution<double> nd_;
};

void AWGN(void);

#endif
