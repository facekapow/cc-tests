const sigRegex = /sig \(big endian\): ([A-Fa-f0-9]+)/
const expRegex = /exp \(big endian\): ([A-Fa-f0-9]+)/
const modRegex = /mod \(big endian\): ([A-Fa-f0-9]+)/
const recipRegex = /mod_recip \(big endian\): ([A-Fa-f0-9]+)/

const process = require('process')
const util = require('./util')
const to_cczp_power_test_data = require('./to-cczp_power-test-data')

/**
 * @param {number} iterations
 * @param {string} url
 */
module.exports = (iterations, url) => {
	let result = []
	for (let i = 0; i < iterations; ++i) {
		const stdout = util.shell(`darling shell curl "https://${url}"`)
	
		const sig = stdout.match(sigRegex)[1]
		const exp = stdout.match(expRegex)[1]
		const mod = stdout.match(modRegex)[1]
		const recip = stdout.match(recipRegex)[1]

		const data = to_cczp_power_test_data(sig, exp, mod, recip)

		result.push(`
cctest_cczp_power("${url} #${i + 1}",
${data}
);
`.trim())
	}
	return result
}


if (require.main === module) {
	if (process.argv.length < 4) {
		console.log('Usage: node scripts/collect-cczp_power-data.js iteration-count url-without-scheme');
		process.exit(1);
	}

	const iterations = parseInt(process.argv[2].trim())
	const url = process.argv[3]

	module.exports(iterations, url)
}
