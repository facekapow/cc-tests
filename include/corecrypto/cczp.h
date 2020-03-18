#ifndef CC_CCZP_H
#define CC_CCZP_H

#include <corecrypto/ccn.h>

struct CC_ALIGNED(CCN_UNIT_SIZE) cczp {
	cc_size n;
	cc_unit options;
	void (*mod_prime)(const struct cczp* zp, cc_unit* r, const cc_unit* s, cc_ws_t ws);
};

struct CC_ALIGNED(CCN_UNIT_SIZE) cczp_short {
	cc_size n;
	cc_unit options;
	void (*mod_prime)(const struct cczp* zp, cc_unit* r, const cc_unit* s, cc_ws_t ws);
};

struct cczp_prime {
	struct cczp hdr;
	cc_unit ccn[];
};

typedef struct cczp* cczp_t;
typedef struct cczp_short* cczp_short_t;
typedef const struct cczp* cczp_const_t;
typedef const struct cczp_short* cczp_const_short_t;

typedef void (ccmod_prime_f)(cczp_const_t, cc_unit *, const cc_unit *, cc_ws_t);

#define cczp_t_AS_u(x) ((cc_unit*)x)
#define cczp_t_AS_zp(x) ((struct cczp*)x)
#define cczp_t_AS_prime(x) ((struct cczp_prime*)x)

#define cczp_short_t_AS_u(x) ((cc_unit*)x)
#define cczp_short_t_AS_zp(x) ((struct cczp*)x)
#define cczp_short_t_AS_prime(x) ((struct cczp_prime*)x)
#define cczp_short_t_AS_szp(x) ((struct cczp_short*)x)
#define cczp_short_t_AS__lzp(x) ((cczp_t)x)

#define cczp_const_t_AS_u(x) ((const cc_unit*)x)
#define cczp_const_t_AS_zp(x) ((const struct cczp*)x)
#define cczp_const_t_AS_prime(x) ((const struct cczp_prime*)x)
#define cczp_const_t_AS__nczp(x) ((cczp_t)x)

#define cczp_const_short_t_AS_u(x) ((const cc_unit *)x)
#define cczp_const_short_t_AS_zp(x) ((const struct cczp *)x)
#define cczp_const_short_t_AS_prime(x) ((struct cczp_prime *)x)
#define cczp_const_short_t_AS_szp(x) ((const struct cczp_short *)x)
#define cczp_const_short_t_AS__clzp(x) ((cczp_const_t)x)
#define cczp_const_short_t_AS__szp(x) ((cczp_short_t)x)
#define cczp_const_short_t_AS__lzp(x) ((cczp_t)x)

#define CCZP_PRIME(ZP) (cczp_short_t_AS_prime((cczp_short_t)(ZP))->ccn)

#define CCZP_RECIP(ZP) (cczp_t_AS_prime((cczp_t)(ZP))->ccn + cczp_n((cczp_const_short_t)ZP))
#define cczp_size(_size_) (sizeof(struct cczp) + ccn_sizeof_n(1) + 2 * (_size_))
#define CCZP_N(ZP) (cczp_short_t_AS_zp((cczp_short_t)(ZP))->n)
#define CCZP_MOD_PRIME(ZP) (cczp_short_t_AS_zp((cczp_short_t)(ZP))->mod_prime)

CC_INLINE
cc_size cczp_n(cczp_const_short_t zp) {
	return cczp_t_AS_zp(zp)->n;
};

CC_INLINE
const cc_unit* cczp_recip(cczp_const_t zp) {
	return cczp_t_AS_u(zp) + cczp_n((cczp_const_short_t)zp) + ccn_nof_size(sizeof(struct cczp));
};

CC_INLINE
const cc_unit* cczp_prime(cczp_const_short_t zp) {
	return cczp_t_AS_u(zp) + ccn_nof_size(sizeof(struct cczp));
};

CC_INLINE
ccmod_prime_f* cczp_mod_prime(cczp_const_t zp) {
	return cczp_t_AS_zp(zp)->mod_prime;
};

void cczp_init(cczp_t zp);

void cczp_mod(cczp_const_t zp, cc_unit* r, const cc_unit* s2n, cc_ws_t ws);

void cczp_power(cczp_const_t zp, cc_unit* r, const cc_unit* m, const cc_unit* e);

#endif // CC_CCZP_H
