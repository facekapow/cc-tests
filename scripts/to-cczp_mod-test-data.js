const { CCN_UNIT_SIZE } = require('./constants')
const process = require('process')
const util = require('./util')

/**
 * @param {string} operand
 * @param {string} modulus
 */
module.exports = (operand, modulus) => {
	const minLen = util.roundUpToNearest(Math.min(operand.length, modulus.length), CCN_UNIT_SIZE * 2)
	const maxLen = util.roundUpToNearest(Math.max(operand.length, modulus.length), CCN_UNIT_SIZE * 2)
	const len = maxLen > minLen * 2 ? (maxLen / 2) : minLen
	const n = len / (CCN_UNIT_SIZE * 2)
	const byteLen = len / 2

	operand = operand.padStart(len * 2, '0')
	modulus = modulus.padStart(len, '0')

	const expectation =
		util.shell(`python -c "print(hex(int('${operand}', 16) % int('${modulus}', 16)))"`)
		.slice(2, -1)
		.padStart(len, '0')

	return `
(cczp_mod_data_t) {
.n = ${n},
.operand = (uint8_t[${byteLen * 2}]) {
${util.toByteArray(operand, true)}
},
.modulus = (uint8_t[${byteLen}]) {
${util.toByteArray(modulus, true)}
},
.expectation = (uint8_t[${byteLen}]) {
${util.toByteArray(expectation, true)}
},
}
`.trim()
}

if (require.main === module) {
	if (process.argv.length < 5) {
		console.log('Usage: node scripts/to-cczp_mod-test-data.js operand-in-big-endian-hex modulus-in-big-endian-hex');
		process.exit(1);
	}

	let operand = process.argv[2].trim()
	let modulus = process.argv[3].trim()

	console.log(module.exports(operand, modulus))
}
