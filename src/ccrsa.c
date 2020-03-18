#include <corecrypto/ccrsa.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccdigest_priv.h>
#include <corecrypto/ccder.h>
#include <stdlib.h>

int ccrsa_emsa_pkcs1v15_encode(size_t emlen, uint8_t* em, size_t dgstlen, const uint8_t* dgst, const uint8_t* oid) {
	// NOTE(@facekapow):
	// I'll be quoting specific steps from RFC 8017 Sec. 9.2

	// 1. Apply the hash function to the message M to produce a hash value H
	//
	// NOTE(@facekapow):
	// we don't have to hash it; `dgst` already contains the hash

	// 2. Encode the algorithm ID for the hash function and the hash
  //    value into an ASN.1 value of type DigestInfo
	//
	// NOTE(@facekapow):
	// we actually just calculate the sizes;
	// we generate the encoded DER structure later on
	size_t alg_info_seq_size = ccder_sizeof_oid(oid) + ccder_sizeof(CCDER_NULL, 0);
	size_t digest_info_seq_size = ccder_sizeof(CCDER_CONSTRUCTED_SEQUENCE, alg_info_seq_size) + ccder_sizeof_raw_octet_string(dgstlen);
	// this is a.k.a. `tLen`
	size_t digest_info_size = ccder_sizeof(CCDER_CONSTRUCTED_SEQUENCE, digest_info_seq_size);

	// 3. If emLen < tLen + 11, output "intended encoded message length
  //    too short" and stop.
	if (emlen < digest_info_size + 11)
		return -1;

	// 4. Generate an octet string PS consisting of emLen - tLen - 3
  //    octets with hexadecimal value 0xff
	uint8_t* ps = em + 2;
	size_t ps_len = emlen - digest_info_size - 3;
	for (size_t i = 0; i < ps_len; ++i)
		ps[i] = 0xff;

	// 5.  Concatenate PS, the DER encoding T, and other padding to form
	//     the encoded message EM as
	//        EM = 0x00 || 0x01 || PS || 0x00 || T
	em[0] = 0x00;
	em[1] = 0x01;
	ps[ps_len] = 0x00;

	// this is where we actually generate the DER structure and hash the message
	// we bake them directly into `em` to avoid unnecessary allocations
	uint8_t* digest_info = ps + ps_len + 1;
	uint8_t* digest_info_end = digest_info + digest_info_size;
	uint8_t* h = digest_info_end - dgstlen;

	memcpy(h, dgst, dgstlen);

	ccder_encode_tl(CCDER_CONSTRUCTED_SEQUENCE, digest_info_seq_size, digest_info,
		ccder_encode_tl(CCDER_CONSTRUCTED_SEQUENCE, alg_info_seq_size, digest_info,
			ccder_encode_oid(oid, digest_info,
			ccder_encode_tl(CCDER_NULL, 0, digest_info,
		ccder_encode_tl(CCDER_OCTET_STRING, dgstlen, digest_info, h)))));

	return 0;
}

/*
 * Verify correctness of signature given. Digest is the data being signed.
 *
 * The return value is > 0 if there is an other error.
 * valid is set to false if there is an invalid signature.
 */
int ccrsa_verify_pkcs1v15(ccrsa_pub_ctx_t key, const uint8_t* oid, size_t digest_len, const uint8_t* digest, size_t sig_len, const uint8_t* sig, bool* valid) {
	cc_println("key", ccrsa_pub_ctx_size(ccn_sizeof_n(ccrsa_ctx_n(key))), (const uint8_t*)cczp_t_AS_u(ccrsa_ctx_zm(key)));
	cc_println("oid", strlen((const char*)oid), oid);
	printf("digest_len: %zu\n", digest_len);
	cc_println_be("digest", digest_len, digest);
	printf("sig_len: %zu\n", sig_len);
	cc_println_be("sig", sig_len, sig);

	*valid = false;

	cc_unit* s = NULL;
	struct cczp* zp = NULL;
	cc_unit* m = NULL;
	uint8_t* em = NULL;
	uint8_t* em_prime = NULL;

	cc_size mod_size = ccrsa_ctx_n(key);
	cc_unit* modulus = ccrsa_ctx_m(key);
	cc_unit* exponent = ccrsa_ctx_e(key);

	// mod_size is how many units are allocated for the modulus
	// mod_len is how many bytes (*bytes*, not units) are in use
	cc_size mod_bits = ccn_bitlen(mod_size, modulus);
	cc_size mod_len = (mod_bits + 7) / 8;
	mod_size = ccn_nof(mod_bits);

	cc_size sig_bits = ccn_bitsof_size(sig_len);

	printf("n of mod: %zu\n", mod_size);
	printf("mod_len: %zu\n", mod_len);
	cc_println_be("mod", ccn_sizeof_n(mod_size), (const uint8_t*)modulus);
	cc_println_be("e", ccn_sizeof_n(mod_size), (const uint8_t*)exponent);

	// Length of signature should equal length of modulus
	if (sig_bits != mod_bits) {
		printf("%s: sig_bits (%zu) != mod_bits (%zu)\n", __PRETTY_FUNCTION__, sig_bits, mod_bits);
		goto fail;
	}

	// Compute big uint representation of signature
	cc_size sig_units = ccn_nof_size(sig_len);
	cc_size sig_bytes = ccn_sizeof_n(sig_units);

	s = __builtin_alloca(sig_bytes);
	memset(s, 0, sig_bytes);
	if (ccn_read_uint(sig_units, s, sig_len, sig)) {
		printf("%s: failed to read signature\n", __PRETTY_FUNCTION__);
		goto fail;
	}

	// Verify that s is in the range of 0 and modulus-1
	if (ccn_cmp(sig_units, s, modulus) >= 0 || ccn_bitlen(sig_units, s) == 0) {
		printf("%s: s not in range of [0, modulus)\n", __PRETTY_FUNCTION__);
		goto fail;
	}

	// m = s^e mod n

	zp = __builtin_alloca(cczp_size(sig_bytes));
	CCZP_N(zp) = sig_units;
	cczp_init(zp);

	// maybe we should copy modulus into cczp_prime
	memcpy(CCZP_PRIME(zp), modulus, ccn_sizeof_n(CCZP_N(zp)));

	m = __builtin_alloca(sig_bytes);
	cczp_power(zp, m, s, exponent);

	em = __builtin_alloca(sig_bytes);
	memset(em, 0, sig_bytes);
	ccn_write_uint(sig_units, m, sig_bytes, em);

	cc_println_be("m", sig_bytes, (const uint8_t*)m);
	cc_println("em", sig_bytes, (const uint8_t*)em);

	em_prime = __builtin_alloca(sig_bytes);
	memset(em_prime, 0, sig_bytes);
	if (ccrsa_emsa_pkcs1v15_encode(sig_bytes, em_prime, digest_len, digest, oid)) {
		printf("%s: failed to encode with EMSA-PKCS#1v1.5\n", __PRETTY_FUNCTION__);
		goto fail;
	}

	cc_println("em_prime", sig_bytes, em_prime);

	*valid = !memcmp(em, em_prime, sig_bytes);
	if (!*valid) {
		printf("%s: em != em_prime\n", __PRETTY_FUNCTION__);
	}

	return 0;

fail:
	printf("%s: failed\n", __PRETTY_FUNCTION__);
	return 1;
};
