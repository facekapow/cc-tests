#include <corecrypto/ccn.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

cc_size ccn_n(cc_size n, const cc_unit* s) {
	// Little-endian, so the leading 0 units go at the end.

	// Hopefully constant time
	cc_size last = 0;
	for (cc_size i = 0; i < n; ++i) {
		if (s[i] != 0)
			last = i;
	}

	return last + 1;
};

cc_unit ccn_shift_right(cc_size n, cc_unit* r, const cc_unit* s, size_t k) {
	cc_unit carry = 0;
	cc_unit temp;

	for (cc_size i = n; i > 0; --i) {
		temp = s[i - 1];
		r[i - 1] = temp >> k;
		r[i - 1] |= carry;
		carry = temp << (CCN_UNIT_BITS - k);
	}

	return carry;
};

cc_unit ccn_shift_left(cc_size n, cc_unit* r, const cc_unit* s, size_t k) {
	cc_unit carry = 0;
	cc_unit temp;

	for (cc_size i = 0; i < n; ++i) {
		temp = s[i];
		r[i] = temp << k;
		r[i] |= carry;
		carry = temp >> (CCN_UNIT_BITS - k);
	}

	return carry;
};

size_t ccn_bitlen(cc_size n, const cc_unit* s) {
	cc_size used = ccn_n(n, s);

	// no units used == no bits used
	if (used == 0)
		return 0;

	cc_unit last_used_unit = s[used - 1];

	// if used unit == 0, return 1
	//
	// this check is necessary because `__builtin_clzl` performs
	// undefined behavior when the argument given is 0
	if (used == 1 && last_used_unit == 0)
		return 1;

	// CLZ counts the number of leading zero bits in the
	// the last unit that we know not to be all zeroes.
	return used * CCN_UNIT_BITS - __builtin_clzl(last_used_unit);
};

int ccn_cmp(cc_size n, const cc_unit* s, const cc_unit* t) {
	for (cc_size i = n; i > 0; --i) {
		if (s[i - 1] < t[i - 1]) {
			return -1;
		} else if (s[i - 1] > t[i - 1]) {
			return 1;
		}
	}
	return 0;
};

cc_unit ccn_sub(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit *t) {
	// Compare to determine if there will be underflow
	// Must be done now because r could be the same as s or t
	cc_unit underflow = ccn_cmp(n, s, t) < 0;

	// Make t one's complement
	cc_unit* t_copy = __builtin_alloca(ccn_sizeof_n(n));
	for (cc_size i = 0; i < n; i++)
		t_copy[i] = ~t[i];

	// Add one to make it two's complement
	ccn_add1(n, t_copy, t_copy, 1);

	// Perform addition between s and the two's complement of t
	ccn_add(n, r, s, t_copy);

	return underflow;
};

cc_unit ccn_sub1(cc_size n, cc_unit* r, const cc_unit* s, cc_unit v) {
	ccn_add1(n, r, s, -1 * v);
	return n < 1 || ((n == 1) && s[0] < v);
};

cc_unit ccn_add(cc_size n, cc_unit* r, const cc_unit* s, const cc_unit* t) {
	cc_unit carry = 0;
	for (cc_size i = 0; i < n; ++i) {
		cc_unit s_current = s[i];
		cc_unit t_current = t[i];
		cc_unit sum = s_current + t_current + carry;
		r[i] = sum;
		// Overflow check
		if (s_current > sum || t_current > sum) {
			carry = 1;
		} else {
			carry = 0;
		}
	}
	return carry;
};

cc_unit ccn_add1(cc_size n, cc_unit *r, const cc_unit *s, cc_unit v) {
	cc_unit last = s[n - 1];
	const cc_unit max = CCN_UNIT_MASK;
	memcpy(r, s, ccn_sizeof_n(n));
	for (cc_size i = 0; i < n; ++i) {
		// Handle overflow
		if (r[i] + v < r[i]) {
			r[i] += v;
			v = 1;
		} else {
			r[i] += v;
			v = 0;
			break;
		}
	}
	return v;
};

#define CC_HI_BITS(x) (x >> (CCN_UNIT_BITS >> 1))
#define CC_LO_BITS(x) (x & (CCN_UNIT_MASK >> (CCN_UNIT_BITS >> 1)))
#define CC_COMBINE(lo, hi) ((hi << (CCN_UNIT_BITS >> 1)) | lo)

void ccn_mul(cc_size n, cc_unit* r_2n, const cc_unit* s, const cc_unit* t) {
	/*
	for (cc_size i = 0; i < n; ++i) {
		cc_unit carry = 0;
		for (cc_size j = 0; j < n; ++j) {
			// multiplication algorithm that prevents overflow
			// based on https://stackoverflow.com/a/1815371
			const cc_unit s_hi = CC_HI_BITS(s[j]);
			const cc_unit s_lo = CC_LO_BITS(s[j]);
			const cc_unit t_hi = CC_HI_BITS(t[i]);
			const cc_unit t_lo = CC_LO_BITS(t[i]);

			cc_unit x = s_lo * t_lo;
			const cc_unit prod_0 = CC_LO_BITS(x);

			x = s_hi * t_lo + CC_HI_BITS(x);
			cc_unit prod_1 = CC_LO_BITS(x);
			cc_unit prod_2 = CC_HI_BITS(x);

			x = prod_1 + s_lo * t_hi;
			prod_1 = CC_LO_BITS(x);

			x = prod_2 + s_hi * t_hi + CC_HI_BITS(x);

			const cc_unit product = CC_COMBINE(prod_0, prod_1);
			// x is the next carry

			const cc_unit sum = product + r_2n[i + j] + carry;
			const bool overflows = product > sum || r_2n[i + j] > sum;
			r_2n[i + j] = sum;
			carry = x;
			if (overflows)
				++carry;
		}
		r_2n[i + n] = carry;
	}
	*/

	// we're using the karatsuba algorithm in base 2^64
	// in the following pseudocode, B is the base of the numbers
	/*
		procedure karatsuba(num1, num2)
			if (num1 < B) and (num2 < B)
					return num1 * num2

			// Calculates the size of the numbers.
			m = max(size_base(num1, B), size_base(num2, B))
			m2 = floor(m / 2)

			// Split the digit sequences in the middle.
			high1, low1 = split_at(num1, m2)
			high2, low2 = split_at(num2, m2)

			// 3 calls made to numbers approximately half the size.
			z0 = karatsuba(low1, low2)
			z1 = karatsuba(high1, high2)
			z2 = karatsuba((low1 + high1), (low2 + high2))

			t0 = (z2 - z0 - z1) * (B ** m2)
			t1 = z1 * (B ** (m2 * 2))
			// this is equivalent to:
			// t0 = (z2 - z0 - z1) << m2
			// t1 = z1 << (m2 * 2)

			return t0 + t1 + z0
	*/
	cc_size s_n = ccn_n(n, s);
	cc_size t_n = ccn_n(n, t);

	memset(r_2n, 0, ccn_sizeof_n(n * 2));

	if (s_n < 2 && t_n < 2) {
		// multiplication algorithm that prevents overflow
		// based on https://stackoverflow.com/a/1815371
		const cc_unit s_hi = CC_HI_BITS(s[0]);
		const cc_unit s_lo = CC_LO_BITS(s[0]);
		const cc_unit t_hi = CC_HI_BITS(t[0]);
		const cc_unit t_lo = CC_LO_BITS(t[0]);

		cc_unit x = s_lo * t_lo;
		const cc_unit prod_0 = CC_LO_BITS(x);

		x = s_hi * t_lo + CC_HI_BITS(x);
		cc_unit prod_1 = CC_LO_BITS(x);
		cc_unit prod_2 = CC_HI_BITS(x);

		x = prod_1 + s_lo * t_hi;
		prod_1 = CC_LO_BITS(x);

		x = prod_2 + s_hi * t_hi + CC_HI_BITS(x);

		r_2n[0] = CC_COMBINE(prod_0, prod_1);
		r_2n[1] = x;

		return;
	}

	cc_size max_n = s_n > t_n ? s_n : t_n;
	cc_size half_n = (max_n + 1) / 2;
	cc_size result_n = max_n * 2;
	cc_size intermediate_mul_n = (half_n * 2) + 2;

	cc_size max_size = ccn_sizeof_n(max_n);
	cc_size half_size = ccn_sizeof_n(half_n);
	cc_size half_size_for_addition = ccn_sizeof_n(half_n + 1);
	cc_size result_size = ccn_sizeof_n(result_n);
	cc_size intermediate_mul_size = ccn_sizeof_n(intermediate_mul_n);
	cc_size hi_half_size = max_size - half_size;

	// half_size_for_addition is used to allocate enough for addition
	// later when we do `hi + lo -> lo`

	cc_unit* s_hi = __builtin_alloca(half_size);
	cc_unit* s_lo = __builtin_alloca(half_size_for_addition);

	cc_unit* t_hi = __builtin_alloca(half_size);
	cc_unit* t_lo = __builtin_alloca(half_size_for_addition);

	memset(s_hi, 0, half_size);
	memset(s_lo, 0, half_size_for_addition);
	memset(t_hi, 0, half_size);
	memset(t_lo, 0, half_size_for_addition);

	memcpy(s_lo, s, half_size);
	memcpy(s_hi, (uint8_t*)s + half_size, hi_half_size);

	memcpy(t_lo, t, half_size);
	memcpy(t_hi, (uint8_t*)t + half_size, hi_half_size);

	cc_unit* z0 = __builtin_alloca(intermediate_mul_size);
	cc_unit* z1 = __builtin_alloca(intermediate_mul_size);
	cc_unit* z2 = __builtin_alloca(intermediate_mul_size);

	memset(z0, 0, intermediate_mul_size);
	memset(z1, 0, intermediate_mul_size);
	memset(z2, 0, intermediate_mul_size);

	ccn_mul(half_n, z0, s_lo, t_lo);
	ccn_mul(half_n, z1, s_hi, t_hi);

	cc_unit s_carry = ccn_add(half_n, s_lo, s_lo, s_hi);
	cc_unit t_carry = ccn_add(half_n, t_lo, t_lo, t_hi);

	s_lo[half_n] = s_carry;
	t_lo[half_n] = t_carry;

	ccn_mul(half_n + 1, z2, s_lo, t_lo);

	cc_unit* t0 = __builtin_alloca(result_size);
	cc_unit* t1 = __builtin_alloca(result_size);

	memset(t0, 0, result_size);
	memset(t1, 0, result_size);

	ccn_sub(intermediate_mul_n, z2, z2, z1);
	ccn_sub(intermediate_mul_n, z2, z2, z0);

	ccn_set(intermediate_mul_n, t0, z2);
	ccn_set(intermediate_mul_n, t1, z1);

	const cc_unit* t0_end = t0 + result_n;
	const cc_unit* t1_end = t1 + result_n;

	// shift t0 to the left by `half_n` units
	// (which in little endian is actually shifting right)
	for (cc_size i = intermediate_mul_n; i > 0; --i) {
		cc_unit* tgt = t0 + half_n + (i - 1);
		if (tgt < t0_end)
			*tgt = t0[i - 1];
	}
	for (cc_size i = 0; i < half_n; ++i)
		t0[i] = 0;

	// shift t1 to the left by `half_n * 2` units
	// (which in little endian is actually shifting right)
	for (cc_size i = intermediate_mul_n; i > 0; --i) {
		cc_unit* tgt = t1 + (half_n * 2) + (i - 1);
		if (tgt < t1_end)
			*tgt = t1[i - 1];
	}
	for (cc_size i = 0; i < half_n * 2; ++i)
		t1[i] = 0;

	ccn_set(intermediate_mul_n, r_2n, z0);

	ccn_add(result_n, r_2n, r_2n, t0);
	ccn_add(result_n, r_2n, r_2n, t1);
};

int ccn_read_uint(cc_size n, cc_unit* r, size_t data_size, const uint8_t* data) {
	// The size of r (n) is passed as the number of cc_unit-s,
	// so we use ccn_sizeof_n() to get its actual byte size.
	size_t sizeof_r = ccn_sizeof_n(n);

	// Start by pre-zeroing r.
	memset(r, 0, sizeof_r);

	// Leading zero bytes are insignificant in big endian,
	// so we can safely skip them.
	for (; data_size > 0 && *data == 0; --data_size, ++data);

	// Now, data_size is the actual number of bytes it will
	// take to write out the data. Return -1 if we don't have
	// that much space.
	if (data_size > sizeof_r)
		return -1;

	// We treat r as little-endian with respect to the order
	// of the cc_unit-s, but the cc_unit-s themselves are
	// native-endian (which still means little-endian on
	// i386/x64).
	for (long ind = data_size - 1; ind >= 0; r++) {
		for (cc_size i = 0; i < CCN_UNIT_SIZE && ind >= 0; i++, ind--) {
			cc_unit c = data[ind];
			*r |= c << (i * 8);
		}
	}

	return 0;
};

void ccn_write_uint(cc_size n, const cc_unit* s, size_t out_size, void* out) {
	uint8_t* data = out;

	memset(data, 0, out_size);

	// CCNs are little endian, and C's bitwise operators are too.
	// thus, to write the output in big endian, we loop over the cc_units
	// backwards, and then over each octet backwards

	size_t data_idx = 0;
	for (size_t ccn_idx = n; ccn_idx > 0; --ccn_idx)
		for (uint8_t octet = CCN_UNIT_SIZE; octet > 0; --octet, ++data_idx)
			data[data_idx] = (s[ccn_idx - 1] >> ((octet - 1) * 8)) & 0xff;
};

int ccn_div_euclid(cc_size nq, cc_unit* q, cc_size nr, cc_unit* r, cc_size na, const cc_unit* a, cc_size nd, const cc_unit* d) {
	cc_unit* tmp_remainder = NULL;

	na = ccn_n(na, a);
	nd = ccn_n(nd, d);

	if (!q)
		nq = na;
	if (!r)
		nr = nd;

	if (nq < na)
		return -1;
	if (nr < nd)
		return -1;

	// if the divisor is greater,
	// we cannot divide!
	if (ccn_cmpn(na, a, nd, d) < 0) {
		// note that here it is ok to copy a into r
		// even though r's size depends on that of d
		// because we've determined that d is bigger than a
		if (r)
			ccn_set(na, r, a);
		if (q)
			ccn_zero(nq, q);
		return 0;
	}

	tmp_remainder = __builtin_alloca(ccn_sizeof_n(na));

	ccn_set(na, tmp_remainder, a);

	cc_size tmp_remainder_curr_n = 0;

	// we do ccn_n on every loop because ccn_cmpn looks at both n's and says one value is greater
	// than the other just based on the n's. therefore, if we want to check for real whether the
	// remainder is greater than the divisor, we need to check how many n's are actually used
	while (ccn_cmpn(tmp_remainder_curr_n = ccn_n(na, tmp_remainder), tmp_remainder, nd, d) >= 0) {
		ccn_subn(tmp_remainder_curr_n, tmp_remainder, tmp_remainder, nd, d);
		if (q)
			ccn_add1(nq, q, q, 1);
	}

	// by this point, tmp_remainder should be smaller than the divisor
	// so it should be ok to copy it into the remainder (which is <= the size of the divisor)
	if (r)
		ccn_set(nr, r, tmp_remainder);

	return 0;

fail:
	return -1;
};

void ccn_set(cc_size n, cc_unit* r, const cc_unit* s) {
	for (size_t i = 0; i < n; ++i) {
		r[i] = s[i];
	}
};

size_t ccn_write_int_size(cc_size n, const cc_unit* s) {
	n = ccn_n(n, s);
	return ccn_sizeof_n(n);
};

size_t ccn_write_uint_size(cc_size n, const cc_unit* s) {
	n = ccn_n(n, s);
	return ccn_sizeof_n(n + 1);
};

void ccn_write_int(cc_size n, const cc_unit* s, size_t out_size, void* out) {
	printf("DARLING CRYPTO STUB: %s\n", __PRETTY_FUNCTION__);
};
