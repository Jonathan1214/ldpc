#include "channel.h"

/**@brief awgn
 * @param[in] 
 * @param[out] double guassian random number
 *
*/
double Channel::awgn() {
    return nd(rng);
}
