#ifndef CC_CCRSA_H
#define CC_CCRSA_H

#include <stdbool.h>
#include <stdint.h>

#include <corecrypto/ccn.h>
#include <corecrypto/cczp.h>

cc_aligned_struct(16) ccrsa_full_ctx;
cc_aligned_struct(16) ccrsa_priv_ctx;
cc_aligned_struct(16) ccrsa_pub_ctx;

typedef struct ccrsa_priv_ctx* ccrsa_priv_ctx_t;
typedef struct ccrsa_full_ctx* ccrsa_full_ctx_t;
typedef struct ccrsa_pub_ctx* ccrsa_pub_ctx_t;

#define ccrsa_priv_ctx_t_AS_zp(x) ((cczp_t)x)
#define ccrsa_priv_ctx_t_AS_priv(x) ((ccrsa_priv_ctx*)x)

#define ccrsa_full_ctx_t_AS_zp(x) ((cczp_t)x)
#define ccrsa_full_ctx_t_AS_full(x) ((ccrsa_full_ctx*)x)

#define ccrsa_pub_ctx_t_AS_zp(x) ((cczp_t)x)
#define ccrsa_pub_ctx_t_AS_pub(x) ((ccrsa_pub_ctx*)x)
#define ccrsa_pub_ctx_t_AS_full(x) ((ccrsa_full_ctx*)x)
#define ccrsa_pub_ctx_t_AS_full_t(x) ((ccrsa_full_ctx_t)x)

// Macros

#define ccrsa_ctx_zm(_ctx_)           (ccrsa_pub_ctx_t_AS_zp((ccrsa_pub_ctx_t)(_ctx_)))
#define ccrsa_ctx_n(_ctx_)            (cczp_t_AS_zp(ccrsa_ctx_zm(_ctx_))->n)
#define ccrsa_ctx_m(_ctx_)            (cczp_t_AS_prime(ccrsa_ctx_zm(_ctx_))->ccn)
#define ccrsa_ctx_e(_ctx_)            (ccrsa_ctx_m(_ctx_) + 2 * ccrsa_ctx_n(_ctx_) + 1)
#define ccrsa_ctx_d(_ctx_)            (ccrsa_ctx_m(((ccrsa_full_ctx_t)_ctx_)) + 3 * ccrsa_ctx_n(_ctx_) + 1)

#define ccrsa_pub_ctx_size(_size_)    (sizeof(struct cczp) + CCN_UNIT_SIZE + 3 * (_size_))

#define ccrsa_ctx_private_zp(PRIVK)   (ccrsa_priv_ctx_t_AS_zp((ccrsa_priv_ctx_t)(PRIVK)))
#define ccrsa_ctx_private_zq(PRIVK)   ((cczp_t)(cczp_t_AS_prime(ccrsa_priv_ctx_t_AS_zp((ccrsa_priv_ctx_t)(PRIVK)))->ccn + 2 * ccrsa_priv_ctx_t_AS_zp(ccrsa_ctx_private_zp(PRIVK))->n + 1))
#define ccrsa_ctx_private_dp(PRIVK)   (cczp_t_AS_prime(ccrsa_priv_ctx_t_AS_zp((ccrsa_priv_ctx_t)(PRIVK)))->ccn + 4 * cczp_t_AS_zp(ccrsa_ctx_private_zp(PRIVK))->n + 2 + ccn_nof_size(sizeof(struct cczp)))
#define ccrsa_ctx_private_dq(PRIVK)   (cczp_t_AS_prime(ccrsa_priv_ctx_t_AS_zp((ccrsa_priv_ctx_t)(PRIVK)))->ccn + 5 * cczp_t_AS_zp(ccrsa_ctx_private_zp(PRIVK))->n + 2 + ccn_nof_size(sizeof(struct cczp)))
#define ccrsa_ctx_private_qinv(PRIVK) (cczp_t_AS_prime(ccrsa_priv_ctx_t_AS_zp((ccrsa_priv_ctx_t)(PRIVK)))->ccn + 6 * cczp_t_AS_zp(ccrsa_ctx_private_zp(PRIVK))->n + 2 + ccn_nof_size(sizeof(struct cczp)))

#define ccrsa_priv_ctx_size(_size_)   ((sizeof(struct cczp) + CCN_UNIT_SIZE) * 2 + 7 * ccn_sizeof((ccn_bitsof_size(_size_) / 2) + 1))

#define ccrsa_ctx_public(FK)          ((ccrsa_pub_ctx_t)(FK))

#define ccrsa_full_ctx_size(_size_)   (ccrsa_pub_ctx_size(_size_) + _size_ + ccrsa_priv_ctx_size(_size_))

#define ccrsa_priv_ctx_decl(_size_, _name_) cc_ctx_decl(ccrsa_priv_ctx, ccrsa_priv_ctx_size(_size_), _name_)

#define ccrsa_full_ctx_decl(_size_, _name_) cc_ctx_decl(ccrsa_full_ctx, ccrsa_full_ctx_size(_size_), _name_)

// Functions

CC_INLINE
ccrsa_priv_ctx_t ccrsa_ctx_private(ccrsa_full_ctx_t fk) {
	uint8_t *p = (uint8_t *)ccrsa_full_ctx_t_AS_full(fk);
	size_t p_size = ccn_sizeof_n(ccrsa_ctx_n(fk));
	p += ccrsa_pub_ctx_size(p_size) + p_size;
	ccrsa_priv_ctx *priv = (ccrsa_priv_ctx *)p;
	return (ccrsa_priv_ctx_t)priv;
};

int ccrsa_emsa_pkcs1v15_encode(size_t emlen, uint8_t* em, size_t dgstlen, const uint8_t* dgst, const uint8_t* oid);

int ccrsa_verify_pkcs1v15(ccrsa_pub_ctx_t key, const uint8_t* oid, size_t digest_len, const uint8_t* digest, size_t sig_len, const uint8_t* sig, bool* valid);

#endif // CC_CCRSA_H
