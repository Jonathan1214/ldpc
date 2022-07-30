#include "channel.h"

/**@brief awgn
 * @param[in] 
 * @param[out] double guassian random number
 *
*/
double Channel::awgn() {
    return nd(rng);
}


llr_width noise;
void AWGN(void)
{
    double random1, random2, random3, random4;
    double G, B;

    random1 = rand_ra() % 10000;
    random2 = rand_ra() % 10000;
    random3 = rand_ra() % 10000;
    random4 = rand_ra() % 10000;
    G = sqrt(-2 * log((random1 / 10000) + (random2 / 10000 / 10000) + 0.000000001));
    B = (random3 / 10000) + (random4 / 10000 / 10000);
    noise = G * cos(2 * PI * B);
}
