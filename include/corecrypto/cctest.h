#ifndef CC_CCTEST_H
#define CC_CCTEST_H

#include <corecrypto/ccrsa.h>
#include <corecrypto/cczp.h>
#include <corecrypto/ccn.h>

typedef struct ccn_mul_data {
	const cc_size n;
	const uint8_t* expectation;
	const uint8_t* a;
	const uint8_t* b;
} ccn_mul_data_t;

typedef struct cczp_mod_data {
	const cc_size n;
	const uint8_t* expectation;
	const uint8_t* operand;
	const uint8_t* modulus;
} cczp_mod_data_t;

typedef struct cczp_power_data {
	const cc_size n;
	const uint8_t* expectation;
	const uint8_t* operand;
	const uint8_t* exponent;
	const uint8_t* modulus;
} cczp_power_data_t;

bool cctest_ccn_mul(const char* label, ccn_mul_data_t data);
bool cctest_cczp_mod(const char* label, cczp_mod_data_t data);
bool cctest_cczp_power(const char* label, cczp_power_data_t data);

#endif // CC_CCTEST_H
