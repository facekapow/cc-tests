#include <corecrypto/ccdigest.h>
#include <corecrypto/ccdigest_priv.h>
#include <string.h>

#include <corecrypto/ccmd2.h>
#include <corecrypto/ccmd4.h>
#include <corecrypto/ccmd5.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>
#include <corecrypto/ccripemd.h>

#define __LITTLE_ENDIAN__ 1

#if defined(__llvm__)
	CC_INLINE
	uint64_t _OSSwapInt64(uint64_t _data) {
		return __builtin_bswap64(_data);
	};
#elif defined(__i386__)
	CC_INLINE
	uint64_t _OSSwapInt64(uint64_t _data) {
		__asm__ ("bswap   %%eax\n\t"
						"bswap   %%edx\n\t"
						"xchgl   %%eax, %%edx"
						: "+A" (_data));
		return _data;
	};
#elif defined(__x86_64__)
	CC_INLINE
	uint64_t _OSSwapInt64(uint64_t _data) {
		__asm__ ("bswap   %0" : "+r" (_data));
		return _data;
	};
#else
	#error unknown architecture
#endif

CC_INLINE
uint32_t _OSSwapInt32(uint32_t _data) {
	#if defined(__llvm__)
		return __builtin_bswap32(_data);
	#else
		__asm__ ("bswap   %0" : "+r" (_data));
		return _data;
	#endif
};

#if defined(__LITTLE_ENDIAN__)
	static uint64_t swap64le(uint64_t v) {
		return v;
	};

	#define swap64be _OSSwapInt64

	static void store32le(uint32_t v, void* dest) {
		memcpy(dest, &v, sizeof(v));
	};
	static void store32be(uint32_t v, void* dest) {
		store32le(_OSSwapInt32(v), dest);
	};
#elif defined(__BIG_ENDIAN__)
	static uint64_t swap64be(uint64_t v) {
		return v;
	};

	#define swap64le _OSSwapInt64

	static void store32be(uint32_t v, void* dest) {
		memcpy(dest, &v, sizeof(v));
	};

	static void store32le(uint32_t v, void* dest) {
		store32be(OSSwapInt32(v), dest);
	};
#else
	#error unknown endianess
#endif

void ccdigest_final_64(const struct ccdigest_info* di, ccdigest_ctx_t ctx, unsigned char* digest, uint64_t (*swap64)(uint64_t), void (*store32)(uint32_t, void*)) {
	uint64_t nbits;

	// Add what we have left in to buffer to the total bits processed
	nbits = ccdigest_nbits(di, ctx) += ccdigest_num(di, ctx) * 8;

	// "Terminating" byte, see SHA1_Final()
	ccdigest_data(di, ctx)[ccdigest_num(di, ctx)++] = 0200;

	// Push in zeroes until there are exactly 56 bytes in the internal buffer
	if (ccdigest_num(di, ctx) != 56) {
		uint8_t zeroes[64];
		int count;

		count = (56 - ccdigest_num(di, ctx) + 64) % 64;
		memset(zeroes, 0, count);

		ccdigest_update(di, ctx, count, zeroes);
	}

	nbits = swap64(nbits);

	// This should flush the block
	ccdigest_update(di, ctx, sizeof(nbits), &nbits);

	for (size_t i = 0; i < di->output_size / sizeof(uint32_t); ++i)
		store32(ccdigest_state_u32(di, ctx)[i], &digest[i*4]);
};

void ccdigest_final_64le(const struct ccdigest_info* di, ccdigest_ctx_t ctx, unsigned char* digest) {
	ccdigest_final_64(di, ctx, digest, swap64le, store32le);
};
void ccdigest_final_64be(const struct ccdigest_info* di, ccdigest_ctx_t ctx, unsigned char* digest) {
	ccdigest_final_64(di, ctx, digest, swap64be, store32be);
};

void ccdigest(const struct ccdigest_info* di, size_t len, const void* data, void* digest) {
	ccdigest_di_decl(di, context);

	ccdigest_init(di, context);
	ccdigest_update(di, context, len, data);
	ccdigest_final(di, context, digest);

	ccdigest_di_clear(di, context);
};

void ccdigest_init(const struct ccdigest_info* di, ccdigest_ctx_t ctx) {
	ccdigest_nbits(di, ctx) = 0;
	ccdigest_num(di, ctx) = 0;
	memcpy(ccdigest_state(di, ctx), di->initial_state, di->state_size);
};

static inline void ccdigest_submit_block(const struct ccdigest_info* di, ccdigest_ctx_t ctx, int nblocks, const void* data) {
	di->compress(ccdigest_state(di, ctx), nblocks, data);
	ccdigest_nbits(di, ctx) += nblocks * di->block_size * 8;
};

void ccdigest_update(const struct ccdigest_info* di, ccdigest_ctx_t ctx, size_t len, const void* data) {
	const uint8_t* ptr = (const uint8_t*) data;

	while (len > 0) {
		int buffered = ccdigest_num(di, ctx);
		// Do we have leftover bytes from last time?
		if (buffered > 0) {
			int tocopy = di->block_size - buffered;
			if (tocopy > len)
				tocopy = len;

			memcpy(ccdigest_data(di, ctx) + buffered, ptr, tocopy);

			buffered += tocopy;
			len -= tocopy;
			ptr += tocopy;

			ccdigest_num(di, ctx) = buffered;

			if (buffered == di->block_size) {
				// Submit to compression
				ccdigest_submit_block(di, ctx, 1, ccdigest_data(di, ctx));
				ccdigest_num(di, ctx) = 0; // nothing left in buffer
			}
		} else if (len >= di->block_size) {
			int nblocks = len / di->block_size;
			int bytes = nblocks * di->block_size;

			ccdigest_submit_block(di, ctx, nblocks, ptr);
			len -= bytes;
			ptr += bytes;
		} else {
			// Buffer the remaining data
			memcpy(ccdigest_data(di, ctx), ptr, len);
			ccdigest_num(di, ctx) = len;
			break;
		}
	}
};

int ccdigest_test(const struct ccdigest_info* di, size_t len, const void* data, const void* digest) {
	return ccdigest_test_chunk(di, len, data, digest, len);
};

int ccdigest_test_chunk(const struct ccdigest_info* di, size_t len, const void* data, const void* digest, size_t chunk) {
	const uint8_t* ptr = (const uint8_t*) data;
	unsigned char* calc_digest = (unsigned char*) __builtin_alloca(di->output_size);
	ccdigest_di_decl(di, context);

	ccdigest_init(di, context);

	for (unsigned long done = 0; done < len; done += chunk) {
		ccdigest_update(di, context, chunk, ptr);
		ptr += chunk;
	}

	ccdigest_final(di, context, calc_digest);

	ccdigest_di_clear(di, context);
	return memcmp(calc_digest, digest, di->output_size) == 0;
};

int ccdigest_test_vector(const struct ccdigest_info* di, const struct ccdigest_vector* v) {
	return ccdigest_test(di, v->len, v->message, v->digest);
};

int ccdigest_test_chunk_vector(const struct ccdigest_info* di, const struct ccdigest_vector* v, size_t chunk) {
	return ccdigest_test_chunk(di, v->len, v->message, v->digest, chunk);
};

const struct ccdigest_info* ccdigest_oid_lookup(ccoid_t oid) {
	#define CC_OID_DI_PTR(x) &cc ## x ## _di
	#define CC_OID_DI_FUNC(x) cc ## x ## _di()
	#define CC_OID_CMP(x) if (ccdigest_oid_equal(x, oid)) { return x; }
	#define CC_OID_CMP_PTR(x) CC_OID_CMP(CC_OID_DI_PTR(x))
	#define CC_OID_CMP_FUNC(x) CC_OID_CMP(CC_OID_DI_FUNC(x))

	CC_OID_CMP_PTR(md2);
	CC_OID_CMP_PTR(md4);
	CC_OID_CMP_FUNC(md5);
	CC_OID_CMP_FUNC(sha1);
	CC_OID_CMP_FUNC(sha224);
	CC_OID_CMP_FUNC(sha256);
	CC_OID_CMP_FUNC(sha384);
	CC_OID_CMP_FUNC(sha512);
	CC_OID_CMP_PTR(rmd128);
	CC_OID_CMP_PTR(rmd160);
	CC_OID_CMP_PTR(rmd256);
	CC_OID_CMP_PTR(rmd320);

	return NULL;
};
