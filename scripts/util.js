const array_chunks = (array, chunk_size) => Array(Math.ceil(array.length / chunk_size)).fill().map((_, index) => index * chunk_size).map(begin => array.slice(begin, begin + chunk_size));
const hexDigits = ['0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'];

const { execSync } = require('child_process')

module.exports = {
	/**
	 * @param {string} hex
	 * @param {boolean} reverse
	 */
	toByteArray(hex, reverse = false) {
		let a = hex.match(/.{1,2}/g)
		if (reverse)
			a.reverse()
		return array_chunks(a.map(x => `0x${x.padStart(2, '0')}`), 8).map(x => x.join(', ') + ',').join('\n')
	},

	/**
	 * @param {string} hex
	 * @param {boolean} reverse
	 */
	toPaddedByteArray(hex, reverse = false) {
		return module.exports.toByteArray(hex.padStart(Math.ceil(hex.length / 2) * 2, '0'), reverse)
	},

	/**
	 * @param {string} hex
	 */
	reverseBytes(hex) {
		return hex.match(/.{1,2}/g).reverse().map(x => x.padStart(2, '0')).join('')
	},

	/**
	 * @param {string} str
	 * @param {boolean} reverse
	 */
	fromByteArray(str, reverse = false) {
		let tmp = str.split(',').filter(x => x.trim().length > 0).map(x => x.trim().substr(2))
		if (reverse)
			tmp.reverse()
		return tmp.join('')
	},

	/**
	 * @param {number} x
	 * @param {number} n
	 */
	roundUpToNearest(x, n) {
		const rem = x % n
		return rem == 0 ? x : (x + (n - rem))
	},

	/**
	 * @param {string} command
	 */
	shell(command) {
		return execSync(command, {
				encoding: 'utf8',
				stdio: ['ignore', 'pipe', 'ignore'],
			})
			.trim()
	},

	/**
	 * inclusive min, exclusive max
	 * @param {number} min
	 * @param {number} max
	 */
	randomInteger(min, max) {
		min = Math.ceil(min)
		max = Math.floor(max)
		return Math.floor(Math.random() * (max - min)) + min
	},

	/**
	 * @param {string} length
	 */
	generateHex(length) {
		let str = ''
		for (let i = 0; i < length; ++i)
			str += hexDigits[module.exports.randomInteger(0, 16)]
		return str
	},
}
