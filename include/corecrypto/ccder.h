#ifndef CC_CCDER_H
#define CC_CCDER_H

#include <corecrypto/ccasn1.h>
#include <corecrypto/ccn.h>
#include <stdint.h>

typedef unsigned long ccder_tag;

enum {
	CCDER_EOL										= CCASN1_EOL,
	CCDER_BOOLEAN								= CCASN1_BOOLEAN,
	CCDER_INTEGER								= CCASN1_INTEGER,
	CCDER_BIT_STRING						= CCASN1_BIT_STRING,
	CCDER_OCTET_STRING					= CCASN1_OCTET_STRING,
	CCDER_NULL									= CCASN1_NULL,
	CCDER_OBJECT_IDENTIFIER			= CCASN1_OBJECT_IDENTIFIER,
	CCDER_OBJECT_DESCRIPTOR			= CCASN1_OBJECT_DESCRIPTOR,
	CCDER_REAL									= CCASN1_REAL,
	CCDER_ENUMERATED						= CCASN1_ENUMERATED,
	CCDER_EMBEDDED_PDV					= CCASN1_EMBEDDED_PDV,
	CCDER_UTF8_STRING						= CCASN1_UTF8_STRING,
	CCDER_SEQUENCE							= CCASN1_SEQUENCE,
	CCDER_SET										= CCASN1_SET,
	CCDER_NUMERIC_STRING				= CCASN1_NUMERIC_STRING,
	CCDER_PRINTABLE_STRING			= CCASN1_PRINTABLE_STRING,
	CCDER_T61_STRING						= CCASN1_T61_STRING,
	CCDER_VIDEOTEX_STRING				= CCASN1_VIDEOTEX_STRING,
	CCDER_IA5_STRING						= CCASN1_IA5_STRING,
	CCDER_UTC_TIME							= CCASN1_UTC_TIME,
	CCDER_GENERALIZED_TIME			= CCASN1_GENERALIZED_TIME,
	CCDER_GRAPHIC_STRING				= CCASN1_GRAPHIC_STRING,
	CCDER_VISIBLE_STRING				= CCASN1_VISIBLE_STRING,
	CCDER_GENERAL_STRING				= CCASN1_GENERAL_STRING,
	CCDER_UNIVERSAL_STRING			= CCASN1_UNIVERSAL_STRING,
	CCDER_BMP_STRING						= CCASN1_BMP_STRING,
	CCDER_HIGH_TAG_NUMBER				= CCASN1_HIGH_TAG_NUMBER,
	CCDER_TELETEX_STRING				= CCDER_T61_STRING,

	CCDER_TAG_MASK							= ((ccder_tag)~0),
	CCDER_TAGNUM_MASK						= ((ccder_tag)~((ccder_tag)7 << (sizeof(ccder_tag)*  8 - 3))),

	CCDER_METHOD_MASK						= ((ccder_tag)1 << (sizeof(ccder_tag)*  8 - 3)),
	CCDER_PRIMITIVE							= ((ccder_tag)0 << (sizeof(ccder_tag)*  8 - 3)),
	CCDER_CONSTRUCTED						= ((ccder_tag)1 << (sizeof(ccder_tag)*  8 - 3)),

	CCDER_CLASS_MASK						= ((ccder_tag)3 << (sizeof(ccder_tag)*  8 - 2)),
	CCDER_UNIVERSAL							= ((ccder_tag)0 << (sizeof(ccder_tag)*  8 - 2)),
	CCDER_APPLICATION						= ((ccder_tag)1 << (sizeof(ccder_tag)*  8 - 2)),
	CCDER_CONTEXT_SPECIFIC			= ((ccder_tag)2 << (sizeof(ccder_tag)*  8 - 2)),
	CCDER_PRIVATE								= ((ccder_tag)3 << (sizeof(ccder_tag)*  8 - 2)),

	CCDER_CONSTRUCTED_SET				= CCDER_SET | CCDER_CONSTRUCTED,
	CCDER_CONSTRUCTED_SEQUENCE	= CCDER_SEQUENCE | CCDER_CONSTRUCTED,
};

size_t ccder_sizeof(ccder_tag tag, size_t len);
size_t ccder_sizeof_implicit_integer(ccder_tag implicit_tag, cc_size n, const cc_unit* s);
size_t ccder_sizeof_implicit_octet_string(ccder_tag implicit_tag, cc_size n, const cc_unit* s);
size_t ccder_sizeof_implicit_raw_octet_string(ccder_tag implicit_tag, size_t s_size);
size_t ccder_sizeof_implicit_uint64(ccder_tag implicit_tag, uint64_t value);
size_t ccder_sizeof_integer(cc_size n, const cc_unit* s);
size_t ccder_sizeof_len(size_t len);
size_t ccder_sizeof_octet_string(cc_size n, const cc_unit* s);
size_t ccder_sizeof_oid(ccoid_t oid);
size_t ccder_sizeof_raw_octet_string(size_t s_size);
size_t ccder_sizeof_tag(ccder_tag tag);
size_t ccder_sizeof_uint64(uint64_t value);

uint8_t* ccder_encode_tag(ccder_tag tag, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_len(size_t len, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_tl(ccder_tag tag, size_t len, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_body_nocopy(size_t size, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_constructed_tl(ccder_tag tag, const uint8_t* body_end, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_oid(ccoid_t oid, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_implicit_integer(ccder_tag implicit_tag, cc_size n, const cc_unit* s, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_integer(cc_size n, const cc_unit* s, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_implicit_uint64(ccder_tag implicit_tag, uint64_t value, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_uint64(uint64_t value, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_implicit_octet_string(ccder_tag implicit_tag, cc_size n, const cc_unit* s, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_octet_string(cc_size n, const cc_unit* s, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_implicit_raw_octet_string(ccder_tag implicit_tag, size_t s_size, const uint8_t* s, const uint8_t* der, uint8_t* der_end);
uint8_t* ccder_encode_raw_octet_string(size_t s_size, const uint8_t* s, const uint8_t* der, uint8_t* der_end);


uint8_t* ccder_encode_body(size_t size, const uint8_t* body, const uint8_t* der, uint8_t* der_end);

const uint8_t* ccder_decode_tag(ccder_tag* tagp, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_len(size_t* lenp, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_tl(ccder_tag expected_tag, size_t* lenp, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_constructed_tl(ccder_tag expected_tag, const uint8_t* *body_end, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_sequence_tl(const uint8_t* *body_end, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_uint_n(cc_size* n, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_uint(cc_size n, cc_unit* r, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_uint64(uint64_t* r, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_seqii(cc_size n, cc_unit* r, cc_unit* s, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_oid(ccoid_t* oidp, const uint8_t* der, const uint8_t* der_end);
const uint8_t* ccder_decode_bitstring(const uint8_t** bit_string, size_t* bit_length, const uint8_t* der, const uint8_t* der_end);

#define CC_EC_OID_SECP192R1 {((unsigned char*)"\x06\x08\x2a\x86\x48\xce\x3d\x03\x01\x01")}
#define CC_EC_OID_SECP256R1 {((unsigned char*)"\x06\x08\x2a\x86\x48\xce\x3d\x03\x01\x07")}
#define CC_EC_OID_SECP224R1 {((unsigned char*)"\x06\x05\x2B\x81\x04\x00\x21")}
#define CC_EC_OID_SECP384R1 {((unsigned char* "\x06\x05\x2B\x81\x04\x00\x22")}
#define CC_EC_OID_SECP521R1 {((unsigned char*)"\x06\x05\x2B\x81\x04\x00\x23")}

#endif // CC_CCDER_H
