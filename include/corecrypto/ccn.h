#ifndef CC_CCN_H
#define CC_CCN_H

#include <corecrypto/cc.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#if !defined(CCN_UNIT_SIZE)
	#if defined(__arm64__) || defined(__x86_64__)  || defined(_WIN64) 
		#define CCN_UNIT_SIZE  8
	#elif defined(__arm__) || defined(__i386__) || defined(_WIN32)
		#define CCN_UNIT_SIZE  4
	#else
		#error undefined architecture
	#endif
#endif // !defined(CCN_UNIT_SIZE)

typedef uint8_t cc_byte;
typedef size_t  cc_size;

#if CCN_UNIT_SIZE == 8
	typedef uint64_t cc_unit;        // 64 bit unit
	#define CCN_LOG2_BITS_PER_UNIT 6 // 2^6 = 64 bits
	#define CC_UNIT_C(x) UINT64_C(x)
#elif CCN_UNIT_SIZE == 4
	typedef uint32_t cc_unit;        // 32 bit unit
	#define CCN_LOG2_BITS_PER_UNIT 5 // 2^5 = 32 bits
	#define CC_UNIT_C(x) UINT32_C(x)
#elif CCN_UNIT_SIZE == 2
	typedef uint16_t cc_unit;        // 16 bit unit
	#define CCN_LOG2_BITS_PER_UNIT 4 // 2^4 = 16 bits
	#define CC_UNIT_C(x) UINT16_C(x)
#elif CCN_UNIT_SIZE == 1
	typedef uint8_t cc_unit;         // 8 bit unit
	#define CCN_LOG2_BITS_PER_UNIT 3 // 2^3 = 8 bits
	#define CC_UNIT_C(x) UINT8_C(x)
#else
	#error invalid CCN_UNIT_SIZE
#endif

#define CCN_UNIT_BITS (sizeof(cc_unit) * 8)
#define CCN_UNIT_MASK ((cc_unit)~0)

typedef struct {
	cc_unit* start;
	cc_unit* end;
} cc_ws;

typedef cc_ws* cc_ws_t;

#define ccn_sizeof_n(_n_) (sizeof(cc_unit) * (_n_))

#define ccn_nof(_bits_) (((_bits_) + CCN_UNIT_BITS - 1) >> CCN_LOG2_BITS_PER_UNIT)

#define ccn_sizeof(_bits_) (ccn_sizeof_n(ccn_nof(_bits_)))

#define ccn_nof_size(_size_) (((_size_) + CCN_UNIT_SIZE - 1) / CCN_UNIT_SIZE)

#define ccn_bitsof_n(_n_) ((_n_) * CCN_UNIT_BITS)

#define ccn_bitsof_size(_size_) ((_size_) * 8)

#define ccn_sizeof_size(_size_) ccn_sizeof_n(ccn_nof_size(_size_))

void ccn_set(cc_size n, cc_unit *r, const cc_unit *s);

CC_INLINE
uint8_t ccn_bit(const cc_unit* const ccn, const cc_size k) {
	return 1 & (ccn[k >> CCN_LOG2_BITS_PER_UNIT] >> (k & (CCN_UNIT_BITS - 1)));
};

CC_INLINE
void ccn_set_bit(cc_unit* const ccn, const cc_size k, const bool v) {
	if (v)
		ccn[k >> CCN_LOG2_BITS_PER_UNIT] |= CC_UNIT_C(1) << (k & (CCN_UNIT_BITS - 1));
	else
		ccn[k >> CCN_LOG2_BITS_PER_UNIT] &= ~(CC_UNIT_C(1) << (k & (CCN_UNIT_BITS - 1)));
};

// operation:
//   unitlen s
// returns:
//   1-based index of most significant non-zero unit of s
// sizing:
//   s = n bits
cc_size ccn_n(cc_size n, const cc_unit* s);

// operation:
//   s >> k -> r
// returns:
//   bits shifted out of s
// constraints:
//   k <= n
// sizing:
//   s = n bits
//   r = n bits
cc_unit ccn_shift_right(cc_size n, cc_unit* r, const cc_unit* s, size_t k);

// operation:
//   s << k -> r
// returns:
//   bits shifted out of s
// constraints:
//   k <= n
// sizing:
//   s = n bits
//   r = n bits
cc_unit ccn_shift_left(cc_size n, cc_unit* r, const cc_unit* s, size_t k);

// operation:
//   bitlen s
// returns:
//   1-based index of most significant non-zero bit of s
// sizing:
//   s = n bits
size_t ccn_bitlen(cc_size n, const cc_unit* s);

// operation:
//   s <=> t
// returns:
//   -1 if s < t
//    0 if s == t
//    1 if s > t
// sizing:
//   s = n bits
//   t = n bits
int ccn_cmp(cc_size n, const cc_unit* s, const cc_unit* t);

// operation:
//   s <=> t
// returns:
//   -1 if s < t
//    0 if s == t
//    1 if s > t
// sizing:
//   s = ns bits
//   t = nt bits
CC_INLINE
int ccn_cmpn(cc_size ns, const cc_unit* s, cc_size nt, const cc_unit* t) {
	if (ns > nt) {
		return 1;
	} else if (ns < nt) {
		return -1;
	}
	return ccn_cmp(ns, s, t);
}

// operation:
//   s - t -> r
// returns:
//   0 if t <= s
//   1 otherwise
// sizing:
//   s = n bits
//   t = n bits
//   r = n bits
cc_unit ccn_sub(cc_size n, cc_unit* r, const cc_unit* s, const cc_unit* t);

// operation:
//   s - v -> r
// returns:
//   0 if v <= s
//   1 otherwise
// sizing:
//   s = n bits
//   v = `sizeof(cc_unit) * 8` bits
//   r = n bits
cc_unit ccn_sub1(cc_size n, cc_unit* r, const cc_unit* s, cc_unit v);

// operation:
//   s - t -> r
// returns:
//   0 if t <= s
//   1 otherwise
// sizing:
//   s = n bits
//   t = nt bits
//   r = n bits
CC_INLINE
cc_unit ccn_subn(cc_size n, cc_unit* r, const cc_unit* s, cc_size nt, const cc_unit* t) {
	assert(n >= nt);
	return ccn_sub1(n - nt, r + nt, s + nt, ccn_sub(nt, r, s, t));
}

// operation:
//   s + t -> r
// returns:
//   0 if result fits in n bits
//   1 otherwise
// sizing:
//   s = n bits
//   t = n bits
//   r = n bits
cc_unit ccn_add(cc_size n, cc_unit* r, const cc_unit* s, const cc_unit* t);

// operation:
//   s + v -> r
// returns:
//   0 if result fits in n bits
//   1 otherwise
// sizing:
//   s = n bits
//   v = `sizeof(cc_unit) * 8` bits
//   r = n bits
cc_unit ccn_add1(cc_size n, cc_unit* r, const cc_unit* s, cc_unit v);

// operation:
//   s + t -> r
// returns:
//   0 if result fits in n bits
//   1 otherwise
// sizing:
//   s = n bits
//   t = nt bits
//   r = n bits
CC_INLINE
cc_unit ccn_addn(cc_size n, cc_unit* r, const cc_unit* s, cc_size nt, const cc_unit* t) {
	assert(n >= nt);
	return ccn_add1(n - nt, r + nt, s + nt, ccn_add(nt, r, s, t));
}

// operation:
//   s * t -> r_2n
// constraints:
//   r_2n must not overlap with s nor t
// sizing:
//   s = n bits
//   t = n bits
//   r_2n = 2n bits
void ccn_mul(cc_size n, cc_unit* r_2n, const cc_unit* s, const cc_unit* t);

int ccn_read_uint(cc_size n, cc_unit* r, size_t data_size, const uint8_t* data);

#define ccn_read_int(n, r, data_size, data) ccn_read_uint(n, r, data_size, data)

size_t ccn_write_uint_size(cc_size n, const cc_unit* s);

void ccn_write_uint(cc_size n, const cc_unit* s, size_t out_size, void* out);

CC_INLINE
cc_size ccn_write_uint_padded(cc_size n, const cc_unit* s, size_t out_size, uint8_t* to) {
	size_t bytesInKey = ccn_write_uint_size(n, s);
	cc_size offset = (out_size > bytesInKey) ? out_size - bytesInKey : 0;

	cc_zero(offset, to);
	ccn_write_uint(n, s, out_size - offset, to + offset);

	return offset;
}

size_t ccn_write_int_size(cc_size n, const cc_unit* s);

void ccn_write_int(cc_size n, const cc_unit* s, size_t out_size, void* out);

CC_INLINE
void ccn_zero(cc_size n, cc_unit* r) {
	cc_zero(ccn_sizeof_n(n),r);
}

// operation:
//   a / d -> q
//   a % d -> r
// returns:
//    0 if succeeded
//   -1 otherwise
// constraints:
//   nq >= unitlen a
//   nr >= unitlen d
// sizing:
//   a = na bits
//   d = nd bits
//   q = nq bits
//   r = nr bits
int ccn_div_euclid(cc_size nq, cc_unit* q, cc_size nr, cc_unit* r, cc_size na, const cc_unit* a, cc_size nd, const cc_unit* d);

// operation:
//   a / d -> q
// returns:
//    0 if succeeded
//   -1 otherwise
// constraints:
//   nq >= unitlen a
// sizing:
//   a = na bits
//   d = nd bits
//   q = nq bits
#define ccn_div(nq, q, na, a, nd, d) ccn_div_euclid(nq, q, 0, NULL, na, a, nd, d)

// operation:
//   a % d -> r
// returns:
//    0 if succeeded
//   -1 otherwise
// constraints:
//   nr >= unitlen d
// sizing:
//   a = na bits
//   d = nd bits
//   r = nr bits
#define ccn_mod(nr, r, na, a, nd, d) ccn_div_euclid(0, NULL, nr, r, na, a, nd, d)

#endif // CC_CCN_H
