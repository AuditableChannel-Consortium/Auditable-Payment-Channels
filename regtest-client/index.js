const request = require('sync-request')

class RegtestUtils {
    constructor(_opts) {
        _opts = _opts || {}
        this.user = _opts.user || 'root'
        this.pass = _opts.pass || '123456'
        this.host = _opts.host || 'www.littleredhat1997.com'
        this.port = _opts.port || '18443'
        this.API = _opts.API || `http://${this.user}:${this.pass}@${this.host}:${this.port}`
    }

    regtest(method, params) {
        const res = request('POST', this.API, {
            json: {
                "jsonrpc": "1.0",
                "id": "curltest",
                "method": method,
                "params": params
            }
        })
        const body = JSON.parse(res.getBody('utf8'))
        return body.result
    }

    mine(number, miner) {
        number = number || 1
        miner = miner || 'bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g'
        return this.regtest("generatetoaddress", [number, miner])
    }

    listUnspent(address) {
        return this.regtest("listunspent", [null, null, [address]])
    }

    broadcast(hex) {
        const txid = this.regtest("sendrawtransaction", [hex])
        this.mine(1) // 挖矿
        return txid
    }
}

module.exports = RegtestUtils
