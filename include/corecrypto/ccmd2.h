#ifndef CC_CCMD2_H
#define CC_CCMD2_H

#include <corecrypto/ccdigest.h>

#define CCMD2_BLOCK_SIZE  16
#define CCMD2_OUTPUT_SIZE 16
#define CCMD2_STATE_SIZE  64

extern const uint32_t ccmd2_initial_state[16];

#define ccmd2_di ccmd2_ltc_di
extern const struct ccdigest_info ccmd2_ltc_di;

void ccmd2_final(const struct ccdigest_info* di, ccdigest_ctx_t, unsigned char* digest);

#endif // CC_CCMD2_H
