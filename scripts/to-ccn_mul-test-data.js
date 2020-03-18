const { CCN_UNIT_SIZE } = require('./constants')
const process = require('process')
const util = require('./util')

/**
 * @param {string} a
 * @param {string} b
 */
module.exports = (a, b) => {
	const len = util.roundUpToNearest(Math.max(a.length, b.length), CCN_UNIT_SIZE * 2)
	const n = len / (CCN_UNIT_SIZE * 2)
	const byteLen = len / 2

	a = a.padStart(len, '0')
	b = b.padStart(len, '0')

	const expectation =
		util.shell(`python -c "print(hex(int('${a}', 16) * int('${b}', 16)))"`)
		.slice(2, -1)
		.padStart(len * 2, '0')

	return `
(ccn_mul_data_t) {
.n = ${n},
.a = (uint8_t[${byteLen}]) {
${util.toByteArray(a, true)}
},
.b = (uint8_t[${byteLen}]) {
${util.toByteArray(b, true)}
},
.expectation = (uint8_t[${byteLen * 2}]) {
${util.toByteArray(expectation, true)}
},
}
`.trim()
}

if (require.main === module) {
	if (process.argv.length < 5) {
		console.log('Usage: node scripts/to-ccn_mul-test-data.js a-in-big-endian-hex b-in-big-endian-hex');
		process.exit(1);
	}

	let a = process.argv[2].trim()
	let b = process.argv[3].trim()

	console.log(module.exports(a, b))
}
