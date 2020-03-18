#ifndef CC_CCDIGEST_PRIV_H
#define CC_CCDIGEST_PRIV_H

#include <corecrypto/ccdigest.h>
#include <corecrypto/ccasn1.h>

void ccdigest_final_common(const struct ccdigest_info* di, ccdigest_ctx_t ctx, void* digest);
void ccdigest_final_64be(const struct ccdigest_info* di, ccdigest_ctx_t, unsigned char* digest);
void ccdigest_final_64le(const struct ccdigest_info* di, ccdigest_ctx_t, unsigned char* digest);

CC_INLINE
bool ccdigest_oid_equal(const struct ccdigest_info* di, ccoid_t oid) {
	if (di->oid == NULL && CCOID(oid) == NULL)
		return true;
	if (di->oid == NULL || CCOID(oid) == NULL)
		return false;
	return ccoid_equal(di->oid, oid);
}

typedef const struct ccdigest_info* (ccdigest_lookup)(ccoid_t oid);

const struct ccdigest_info* ccdigest_oid_lookup(ccoid_t oid);

#endif // CC_CCDIGEST_PRIV_H
