const { CCN_UNIT_SIZE } = require('./constants')
const process = require('process')
const util = require('./util')

/**
 * @param {string} sig
 * @param {string} exp
 * @param {string} mod
 * @param {string} recip
 */
module.exports = (sig, exp, mod, recip) => {
	const len = util.roundUpToNearest(Math.max(sig.length, exp.length, mod.length), CCN_UNIT_SIZE * 2)
	const n = len / (CCN_UNIT_SIZE * 2)
	const byteLen = len / 2

	sig = sig.padStart(len, '0')
	exp = exp.padStart(len, '0')
	mod = mod.padStart(len, '0')
	recip = recip.padStart(len + 2, '0')

	const expectation =
		util.shell(`python -c "print(hex(pow(int('${sig}', 16), int('${exp}', 16), int('${mod}', 16))))"`)
		.slice(2, -1)
		.padStart(len, '0')

	return `
(cczp_power_data_t) {
.n = ${n},
.operand = (uint8_t[${byteLen}]) {
${util.toByteArray(sig, true)}
},
.exponent = (uint8_t[${byteLen}]) {
${util.toByteArray(exp, true)}
},
.modulus = (uint8_t[${byteLen}]) {
${util.toByteArray(mod, true)}
},
.reciprocal = (uint8_t[${byteLen + CCN_UNIT_SIZE}]) {
${util.toByteArray(recip, true)}
},
.expectation = (uint8_t[${byteLen}]) {
${util.toByteArray(expectation, true)}
},
}
`.trim()
}

if (require.main === module) {
	if (process.argv.length < 5) {
		console.log('Usage: node scripts/to-cczp_power-test-data.js signature-in-big-endian-hex exponent-in-big-endian-hex modulus-in-big-endian-hex');
		process.exit(1);
	}

	let sig = process.argv[2].trim()
	let exp = process.argv[3].trim()
	let mod = process.argv[4].trim()

	console.log(module.exports(sig, exp, mod))
}
