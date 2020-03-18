#ifndef CC_CCASN1_H
#define CC_CCASN1_H

#include <corecrypto/cc.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

enum {
	CCASN1_EOL									= 0x00,
	CCASN1_BOOLEAN							= 0x01,
	CCASN1_INTEGER							= 0x02,
	CCASN1_BIT_STRING						= 0x03,
	CCASN1_OCTET_STRING					= 0x04,
	CCASN1_NULL									= 0x05,
	CCASN1_OBJECT_IDENTIFIER		= 0x06,
	CCASN1_OBJECT_DESCRIPTOR		= 0x07,
	CCASN1_REAL									= 0x09,
	CCASN1_ENUMERATED						= 0x0a,
	CCASN1_EMBEDDED_PDV					= 0x0b,
	CCASN1_UTF8_STRING					= 0x0c,
	CCASN1_SEQUENCE							= 0x10,
	CCASN1_SET									= 0x11,
	CCASN1_NUMERIC_STRING				= 0x12,
	CCASN1_PRINTABLE_STRING			= 0x13,
	CCASN1_T61_STRING						= 0x14,
	CCASN1_VIDEOTEX_STRING			= 0x15,
	CCASN1_IA5_STRING						= 0x16,
	CCASN1_UTC_TIME							= 0x17,
	CCASN1_GENERALIZED_TIME			= 0x18,
	CCASN1_GRAPHIC_STRING				= 0x19,
	CCASN1_VISIBLE_STRING				= 0x1a,
	CCASN1_GENERAL_STRING				= 0x1b,
	CCASN1_UNIVERSAL_STRING			= 0x1c,
	CCASN1_BMP_STRING						= 0x1e,
	CCASN1_HIGH_TAG_NUMBER			= 0x1f,
	CCASN1_TELETEX_STRING				= CCASN1_T61_STRING,

	CCASN1_TAG_MASK							= 0xff,
	CCASN1_TAGNUM_MASK					= 0x1f,

	CCASN1_METHOD_MASK					= 0x20,
	CCASN1_PRIMITIVE						= 0x00,
	CCASN1_CONSTRUCTED					= 0x20,

	CCASN1_CLASS_MASK						= 0xc0,
	CCASN1_UNIVERSAL						= 0x00,
	CCASN1_APPLICATION					= 0x40,
	CCASN1_CONTEXT_SPECIFIC			= 0x80,
	CCASN1_PRIVATE							= 0xc0,

	CCASN1_CONSTRUCTED_SET			= CCASN1_SET | CCASN1_CONSTRUCTED,
	CCASN1_CONSTRUCTED_SEQUENCE	= CCASN1_SEQUENCE | CCASN1_CONSTRUCTED,
};

typedef const unsigned char * ccoid_t;

#define CCOID(oid) (oid)

ccoid_t ccoid_for_der(size_t* der_len, const uint8_t** der);

CC_INLINE
size_t ccoid_size(ccoid_t oid) {
	return 2 + CCOID(oid)[1];
};

CC_INLINE
bool ccoid_equal(ccoid_t oid1, ccoid_t oid2) {
	return (ccoid_size(oid1) == ccoid_size(oid2) && memcmp(CCOID(oid1), CCOID(oid2), ccoid_size(oid1))== 0);
};

#endif // CC_CCASN1_H
