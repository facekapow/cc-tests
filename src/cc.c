#include <corecrypto/cctest.h>
#include <stdio.h>
#include <stdlib.h>

void cc_clear(size_t len, void *dst) {
	volatile unsigned char *ptr = (volatile unsigned char*)dst;
	while (len--)
		*ptr++ = 0;
};

int main(int argc, char** argv) {
	#include "../gen/cczp_power-test-data.c"
	#include "../gen/cczp_mod-test-data.c"
	#include "../gen/ccn_mul-test-data.c"
};
