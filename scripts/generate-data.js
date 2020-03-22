const { CCN_UNIT_SIZE } = require('./constants')
const urls = [
	'www.apple.com',
	'www.example.com',
	'www.microsoft.com',
]
const maxN = 32
const urlTestCount = 5
const testCount = 15

const { createWriteStream } = require('fs')
const util = require('./util')
const collect_cczp_power_data = require('./collect-cczp_power-data')
const to_ccn_mul_test_data = require('./to-ccn_mul-test-data')
const to_ccn_shift_right_test_data = require('./to-ccn_shift_right-test-data')

const ccn_mul_data = createWriteStream(`${__dirname}/../gen/ccn_mul-test-data.c`)
const cczp_power_data = createWriteStream(`${__dirname}/../gen/cczp_power-test-data.c`)
const ccn_shift_right_data = createWriteStream(`${__dirname}/../gen/ccn_shift_right-test-data.c`)

console.log('collecting (sig, exp, mod) data...')

for (const url of urls)
	cczp_power_data.write(collect_cczp_power_data(urlTestCount, url).join('\n'))

console.log('generating ccn_mul data...')

for (let i = 0; i < testCount; ++i) {
	const data = to_ccn_mul_test_data(
		util.generateHex(maxN * CCN_UNIT_SIZE * 2),
		util.generateHex(maxN * CCN_UNIT_SIZE * 2)
	)
	ccn_mul_data.write(`
cctest_ccn_mul("iteration #${i + 1}",
${data}
);
`.trim() + '\n')
}

console.log('generating ccn_shift_right data...')

for (let i = 0; i < testCount; ++i) {
	const data = to_ccn_shift_right_test_data(
		util.generateHex(maxN * CCN_UNIT_SIZE * 2),
		1//util.randomInteger(0, maxN)
	)
	ccn_shift_right_data.write(`
cctest_ccn_shift_right("iteration #${i + 1}",
${data}
);
`.trim() + '\n')
}

ccn_mul_data.end()
cczp_mod_data.end()
cczp_power_data.end()
ccn_shift_right_data.end()

console.log('done.')
