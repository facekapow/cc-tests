#ifndef CC_CCSHA1_H
#define CC_CCSHA1_H

#include <corecrypto/ccdigest.h>

#define CCSHA1_BLOCK_SIZE  64
#define CCSHA1_OUTPUT_SIZE 20
#define CCSHA1_STATE_SIZE  20

const struct ccdigest_info* ccsha1_di();

extern const uint32_t ccsha1_initial_state[5];

void ccsha1_final(const struct ccdigest_info *di, ccdigest_ctx_t, unsigned char *digest);

extern const struct ccdigest_info ccsha1_ltc_di;
extern const struct ccdigest_info ccsha1_eay_di;

#define ccoid_sha1 ((unsigned char *)"\x06\x05\x2b\x0e\x03\x02\x1a")
#define ccoid_sha1_len 7

#endif // CC_CCSHA1_H
