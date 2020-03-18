#ifndef CC_CCMD4_H
#define CC_CCMD4_H

#include <corecrypto/ccdigest.h>

#define CCMD4_BLOCK_SIZE  64
#define CCMD4_OUTPUT_SIZE 16
#define CCMD4_STATE_SIZE  88

extern const uint32_t ccmd4_initial_state[4];

#define ccmd4_di ccmd4_ltc_di
extern const struct ccdigest_info ccmd4_ltc_di;

#endif // CC_CCMD4_H
