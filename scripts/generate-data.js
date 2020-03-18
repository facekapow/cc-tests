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
const to_ccn_mul_test_data = require('./to-ccn_mul-test-data')
const to_cczp_mod_test_data = require('./to-cczp_mod-test-data')
const to_cczp_power_test_data = require('./to-cczp_power-test-data')
const collect_cczp_power_data = require('./collect-cczp_power-data')

const ccn_mul_data = createWriteStream(`${__dirname}/../gen/ccn_mul-test-data.c`)
const cczp_mod_data = createWriteStream(`${__dirname}/../gen/cczp_mod-test-data.c`)
const cczp_power_data = createWriteStream(`${__dirname}/../gen/cczp_power-test-data.c`)

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

console.log('generating cczp_mod data...')

for (let i = 0; i < testCount; ++i) {
	const data = to_cczp_mod_test_data(
		util.generateHex(maxN * CCN_UNIT_SIZE * 4),
		util.generateHex(maxN * CCN_UNIT_SIZE * 2)
	)
	cczp_mod_data.write(`
cctest_cczp_mod("iteration #${i + 1}",
${data}
);
`.trim() + '\n')
}

console.log('generating cczp_power data...')

for (let i = 0; i < testCount; ++i) {
	const data = to_cczp_power_test_data(
		util.generateHex(maxN * CCN_UNIT_SIZE * 2),
		util.generateHex(maxN * CCN_UNIT_SIZE * 2),
		util.generateHex(maxN * CCN_UNIT_SIZE * 2)
	)
	cczp_power_data.write(`
cctest_cczp_power("iteration #${i + 1}",
${data}
);
`.trim() + '\n')
}

ccn_mul_data.end()
cczp_mod_data.end()
cczp_power_data.end()

console.log('done.')
