const { CCN_UNIT_SIZE } = require('./constants')
const process = require('process')
const util = require('./util')

/**
 * @param {string} operand
 * @param {number} k
 */
module.exports = (operand, k) => {
	const len = util.roundUpToNearest(operand.length, CCN_UNIT_SIZE * 2)
	const n = len / (CCN_UNIT_SIZE * 2)
	const byteLen = len / 2

	operand = operand.padStart(len, '0')

	const expectation =
		util.shell(`python -c "print(hex(int('${operand}', 16) >> ${k}))"`)
		.slice(2, -1)
		.padStart(len, '0')

	return `
(ccn_shift_right_data_t) {
.n = ${n},
.operand = (uint8_t[${byteLen}]) {
${util.toByteArray(operand, true)}
},
.k = ${k},
.expectation = (uint8_t[${byteLen}]) {
${util.toByteArray(expectation, true)}
},
}
`.trim()
}

if (require.main === module) {
	if (process.argv.length < 5) {
		console.log('Usage: node scripts/to-ccn_mul-test-data.js operand-in-big-endian-hex k-as-decimal');
		process.exit(1);
	}

	let operand = process.argv[2].trim()
	let k = parseInt(process.argv[3].trim())

	console.log(module.exports(operand, k))
}
