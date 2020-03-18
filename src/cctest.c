#include <corecrypto/cctest.h>
#include <corecrypto/ccn.h>

#define ANSI_BLACK    "\033[0;30m"
#define ANSI_RED      "\033[0;31m"
#define ANSI_GREEN    "\033[0;32m"
#define ANSI_YELLOW   "\033[0;33m"
#define ANSI_BLUE     "\033[0;34m"
#define ANSI_MAGENTA  "\033[0;35m"
#define ANSI_CYAN     "\033[0;36m"
#define ANSI_GREY     "\033[0;37m"
#define ANSI_DARKGREY "\033[01;30m"
#define ANSI_BRED     "\033[01;31m"
#define ANSI_BGREEN   "\033[01;32m"
#define ANSI_BYELLOW  "\033[01;33m"
#define ANSI_BBLUE    "\033[01;34m"
#define ANSI_BMAGENTA "\033[01;35m"
#define ANSI_BCYAN    "\033[01;36m"
#define ANSI_WHITE    "\033[01;37m"
#define ANSI_NORMAL   "\033[0m"
#define ANSI_BOLD     "\033[01m"

bool cctest_ccn_mul(const char* label, ccn_mul_data_t data) {
	printf("ccn_mul:%s - %s...%s", label, ANSI_BOLD, ANSI_NORMAL);
	fflush(stdout);

	uint8_t* result = __builtin_alloca(ccn_sizeof_n(data.n * 2));

	ccn_mul(data.n, (cc_unit*)result, (const cc_unit*)data.a, (const cc_unit*)data.b);

	bool valid = !memcmp(result, data.expectation, ccn_sizeof_n(data.n * 2));
	printf("\rccn_mul:%s - %s%s%s\n", label, valid ? ANSI_BGREEN : ANSI_BRED, valid ? "PASS" : "FAIL", ANSI_NORMAL);
	return valid;
};

bool cctest_cczp_mod(const char* label, cczp_mod_data_t data) {
	printf("cczp_mod:%s - %s...%s", label, ANSI_BOLD, ANSI_NORMAL);
	fflush(stdout);

	uint8_t* result = __builtin_alloca(ccn_sizeof_n(data.n + 1 / 2));

	cczp_t zp = __builtin_alloca(cczp_size(ccn_sizeof_n(data.n)));
	CCZP_N(zp) = data.n;
	cczp_init(zp);

	// maybe we should copy modulus into cczp_prime
	memcpy(CCZP_PRIME(zp), data.modulus, ccn_sizeof_n(CCZP_N(zp)));

	cczp_mod(zp, (cc_unit*)result, (const cc_unit*)data.operand, NULL);

	bool valid = !memcmp(result, data.expectation, ccn_sizeof_n(data.n + 1 / 2));
	printf("\rcczp_mod:%s - %s%s%s\n", label, valid ? ANSI_BGREEN : ANSI_BRED, valid ? "PASS" : "FAIL", ANSI_NORMAL);
	return valid;
};

bool cctest_cczp_power(const char* label, cczp_power_data_t data) {
	printf("cczp_power:%s - %s...%s", label, ANSI_BOLD, ANSI_NORMAL);
	fflush(stdout);

	uint8_t* result = __builtin_alloca(ccn_sizeof_n(data.n));

	cczp_t zp = __builtin_alloca(cczp_size(ccn_sizeof_n(data.n)));
	CCZP_N(zp) = data.n;
	cczp_init(zp);

	// maybe we should copy modulus into cczp_prime
	memcpy(CCZP_PRIME(zp), data.modulus, ccn_sizeof_n(CCZP_N(zp)));

	cczp_power(zp, (cc_unit*)result, (const cc_unit*)data.operand, (const cc_unit*)data.exponent);

	int memcmp_result = memcmp(result, data.expectation, ccn_sizeof_n(data.n));
	bool valid = !memcmp_result;
	printf("\rcczp_power:%s - %s%s%s\n", label, valid ? ANSI_BGREEN : ANSI_BRED, valid ? "PASS" : "FAIL", ANSI_NORMAL);
	return valid;
};
