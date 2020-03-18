#ifndef CC_CCMD5_H
#define CC_CCMD5_H

#include <corecrypto/ccdigest.h>

#define CCMD5_BLOCK_SIZE  64
#define CCMD5_OUTPUT_SIZE 16
#define CCMD5_STATE_SIZE  16

extern const uint32_t ccmd5_initial_state[4];

const struct ccdigest_info* ccmd5_di();

extern const struct ccdigest_info ccmd5_ltc_di;

#endif // CC_CCMD5_H
