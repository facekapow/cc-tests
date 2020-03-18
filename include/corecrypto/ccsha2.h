#ifndef CC_CCSHA2_H
#define CC_CCSHA2_H

#include <corecrypto/ccdigest.h>

const struct ccdigest_info* ccsha224_di(void);
const struct ccdigest_info* ccsha256_di(void);
const struct ccdigest_info* ccsha384_di(void);
const struct ccdigest_info* ccsha512_di(void);

#define ccoid_sha224 ((unsigned char *)"\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x04")
#define ccoid_sha224_len 11

#define ccoid_sha256 ((unsigned char *)"\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x01")
#define ccoid_sha256_len 11

#define ccoid_sha384 ((unsigned char *)"\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x02")
#define ccoid_sha384_len 11

#define ccoid_sha512 ((unsigned char *)"\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x03")
#define ccoid_sha512_len 11

#define CCSHA256_BLOCK_SIZE  64
#define	CCSHA256_OUTPUT_SIZE 32
#define	CCSHA256_STATE_SIZE  32
extern const struct ccdigest_info ccsha256_ltc_di;
extern const struct ccdigest_info ccsha256_v6m_di;
extern const uint32_t ccsha256_K[64];
extern const uint64_t ccsha512_K[80];

#define	CCSHA224_OUTPUT_SIZE 28
extern const struct ccdigest_info ccsha224_ltc_di;

#define CCSHA512_BLOCK_SIZE  128
#define	CCSHA512_OUTPUT_SIZE 64
#define	CCSHA512_STATE_SIZE  64
extern const struct ccdigest_info ccsha512_ltc_di;

#define	CCSHA384_OUTPUT_SIZE 48
extern const struct ccdigest_info ccsha384_ltc_di;

#endif // CC_CCSHA2_H
