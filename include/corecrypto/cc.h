#ifndef CC_CC_H
#define CC_CC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void cc_clear(size_t len, void *dst);

#define cc_zero(_size_, _data_) memset((_data_), 0, (_size_))

#define CC_INLINE static inline

#if defined(_MSC_VER)
	#define CC_ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
	#define CC_ALIGNED(x) __attribute__((aligned(x)))
#endif

#define cc_aligned_struct(_alignment_) \
	typedef CC_ALIGNED(_alignment_) struct {\
		uint8_t b[_alignment_];\
	}

#define cc_ctx_n(_type_, _size_) ((_size_ + sizeof(_type_) - 1) / sizeof(_type_))

#if defined(_MSC_VER)
	#define UNIQUE_ARRAY(data_type, _var_, total_count) data_type* _var_ = (data_type*)_alloca(sizeof(data_type)*(total_count));
	#define cc_ctx_decl(_type_, _size_, _name_) UNIQUE_ARRAY(_type_, _name_,cc_ctx_n(_type_, (_size_)))
#else
	#define cc_ctx_decl(_type_, _size_, _name_) _type_ _name_ [cc_ctx_n(_type_, _size_)]
#endif

#define	CC_STORE32_LE(x, y) do {																	\
    ((unsigned char *)(y))[3] = (unsigned char)(((x)>>24)&255);		\
    ((unsigned char *)(y))[2] = (unsigned char)(((x)>>16)&255);		\
    ((unsigned char *)(y))[1] = (unsigned char)(((x)>>8)&255);		\
    ((unsigned char *)(y))[0] = (unsigned char)((x)&255);					\
} while(0)

#define	CC_LOAD32_LE(x, y) do {																	\
x = ((uint32_t)(((const unsigned char *)(y))[3] & 255)<<24) |		\
    ((uint32_t)(((const unsigned char *)(y))[2] & 255)<<16) |		\
    ((uint32_t)(((const unsigned char *)(y))[1] & 255)<<8)  |		\
    ((uint32_t)(((const unsigned char *)(y))[0] & 255));				\
} while(0)

// MARK: -- 64 bits - little endian

#define	CC_STORE64_LE(x, y) do {																	\
		((unsigned char *)(y))[7] = (unsigned char)(((x)>>56)&255);		\
		((unsigned char *)(y))[6] = (unsigned char)(((x)>>48)&255);		\
		((unsigned char *)(y))[5] = (unsigned char)(((x)>>40)&255);		\
		((unsigned char *)(y))[4] = (unsigned char)(((x)>>32)&255);		\
		((unsigned char *)(y))[3] = (unsigned char)(((x)>>24)&255);		\
		((unsigned char *)(y))[2] = (unsigned char)(((x)>>16)&255);		\
		((unsigned char *)(y))[1] = (unsigned char)(((x)>>8)&255);		\
		((unsigned char *)(y))[0] = (unsigned char)((x)&255);					\
	} while(0)

#define	CC_LOAD64_LE(x, y) do {																			\
	x = (((uint64_t)(((const unsigned char *)(y))[7] & 255))<<56) |		\
			(((uint64_t)(((const unsigned char *)(y))[6] & 255))<<48) |		\
			(((uint64_t)(((const unsigned char *)(y))[5] & 255))<<40) |		\
			(((uint64_t)(((const unsigned char *)(y))[4] & 255))<<32) |		\
			(((uint64_t)(((const unsigned char *)(y))[3] & 255))<<24) |		\
			(((uint64_t)(((const unsigned char *)(y))[2] & 255))<<16) |		\
			(((uint64_t)(((const unsigned char *)(y))[1] & 255))<<8)  |		\
			(((uint64_t)(((const unsigned char *)(y))[0] & 255)));				\
	} while(0)

CC_INLINE
void cc_print(const char* label, size_t count, const uint8_t* array) {
	printf("%s: ", label);
	for (size_t i = 0; i < count; ++i)
		printf("%02x", array[i]);
};

CC_INLINE
void cc_print_be(const char* label, size_t size, const uint8_t* array) {
	printf("%s: ", label);
	for (size_t i = size; i > 0; --i)
		printf("%02x", array[i - 1]);
};

CC_INLINE
void cc_println(const char* label, size_t count, const uint8_t* array) {
	cc_print(label, count, array);
	putchar('\n');
};

CC_INLINE
void cc_println_be(const char* label, size_t count, const uint8_t* array) {
	cc_print_be(label, count, array);
	putchar('\n');
};

#endif // CC_CC_H
