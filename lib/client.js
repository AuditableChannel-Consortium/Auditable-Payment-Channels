const bitcoin = require('bitcoinjs-lib')
const CryptoJS = require('crypto-js')
const BTC = 100000000
const HOURS_IN_DAY = 24
const MINUTES_IN_HOUR = 60
const SECONDS_IN_MINUTE = 60
const ONE_DAY = HOURS_IN_DAY * MINUTES_IN_HOUR * SECONDS_IN_MINUTE

function Client(clientWif, network) {
    this._network = network ? network : bitcoin.networks.regtest
    this._clientKeyPair = bitcoin.ECPair.fromWIF(clientWif, this._network)
    this._clientPubKey = this._clientKeyPair.publicKey
    this._clientAddress = bitcoin.payments.p2wpkh({ pubkey: this._clientPubKey, network: this._network }).address

    // 已经支付的数量
    this._sentAmount = 0
    // 嵌入信息
    this._data = undefined
}

Client.prototype.createCommitment = function (opts) {
    const properties = [
        'serverPubKey',
        'utxos',
        'commitmentAmount',
        'txFee'
    ];

    properties.forEach(function(prop) {
        if (!opts.hasOwnProperty(prop)) {
            throw new Error('参数不存在：' + prop + '')
        }
    })

    if (opts.serverPubKey instanceof String) {
        this._serverPubKey = new Buffer(opts.serverPubKey, 'hex')
    } else if (opts.serverPubKey instanceof Buffer) {
        this._serverPubKey = opts.serverPubKey
    } else {
        throw new Error('支付通道：serverPubKey无效！')
    }
    this._serverAddress = bitcoin.payments.p2wpkh({ pubkey: this._serverPubKey, network: this._network }).address
    this._utxos = opts.utxos
    this._commitmentAmount = opts.commitmentAmount * BTC
    this._txFee = opts.txFee * BTC

    // 找零数量
    const totalAmount = Math.round(this._utxos.reduce((sum, { amount }) => sum + amount, 0) * BTC)
    const changeAmount = totalAmount - (this._commitmentAmount + this._txFee)
    if (changeAmount < 0) {
        throw new Error('支付通道：比特币数量不足！')
    }

    // 多签输入
    const multiSigScriptKeys = [this._clientPubKey, this._serverPubKey]
    const multiSigOutputScript = bitcoin.payments.p2ms({m: 2, pubkeys: multiSigScriptKeys, network: this._network})
    const multiSigScriptHash = bitcoin.payments.p2sh({redeem: multiSigOutputScript, network: this._network})
    this._redeemScript = multiSigOutputScript.output
    this._commitmentAddress = multiSigScriptHash.address // 支付通道地址

    // 构造支付通道
    this.commitmentTx = new bitcoin.Psbt({ network: this._network })

    this._utxos.forEach(element => {
        this.commitmentTx.addInput({
            hash: element.txid,
            index: element.vout,
            witnessUtxo: {
                script: Buffer.from(element.scriptPubKey, 'hex'),
                value: Math.round(element.amount * BTC)
            }
        })
    })

    this.commitmentTx.addOutput({
        address: this._commitmentAddress,
        value: this._commitmentAmount
    })

    this.commitmentTx.addOutput({
        address: this._clientAddress,
        value: changeAmount
    })

    // 支付人签名
    for (let i = 0; i < this._utxos.length; i++) {
        this.commitmentTx.signInput(i, this._clientKeyPair)
    }
    this.commitmentTx.validateSignaturesOfAllInputs()
    this.commitmentTx.finalizeAllInputs()
    return this.commitmentTx.extractTransaction().toHex()
}

Client.prototype.createRefund = function (lockTime) {
    // 时间锁
    lockTime = lockTime || Math.round(new Date().getTime() / 1000) + ONE_DAY

    // 支付通道UTXO
    const MultiSigUTXO = this.getMultiSigUTXO()

    // 扣掉手续费，其余全部退款
    const refundAmount = MultiSigUTXO.amount - this._txFee

    // 构造退款交易
    this.refundTx = new bitcoin.Psbt({network: this._network})
        .setLocktime(lockTime)
        .addInput({
            hash: MultiSigUTXO.txid,
            index: MultiSigUTXO.vout,
            sequence: 0,
            nonWitnessUtxo: Buffer.from(MultiSigUTXO.hex, 'hex'),
            redeemScript: Buffer.from(MultiSigUTXO.redeemScript, 'hex')
        })
        .addOutput({
            address: this._clientAddress,
            value: refundAmount
        })

    // 支付人签名（可以先不签，需要退款的时候再签）
    this.refundTx.signInput(0, this._clientKeyPair)
    this.refundTx.validateSignaturesOfInput(0, this._clientPubKey)
    return this.refundTx.toHex()
}

Client.prototype.incrementPayment = function (amount, data) {
    // 支付通道UTXO
    const MultiSigUTXO = this.getMultiSigUTXO()

    // 检查是否符合要求
    const needSendAmount = this._sentAmount + Math.round(amount * BTC)
    if (this._commitmentAmount < needSendAmount) {
        throw new Error(`支付通道：本次交易比特币超过支付通道数量！ ${this._commitmentAmount} < ${needSendAmount}`)
    }
    const changeAmount = MultiSigUTXO.amount - (needSendAmount + this._txFee)
    if (changeAmount < 0) {
        throw new Error(`支付通道：本次交易比特币超过支付通道数量！ ${changeAmount} < 0`)
    }
    this._sentAmount = needSendAmount
    console.log('\033[91m', `client send: ${amount} BTC, allSend: ${needSendAmount / BTC}`, '\033[0m')

    // 构造支付交易
    this.paymentTx = new bitcoin.Psbt({network: this._network})
        .addInput({
            hash: MultiSigUTXO.txid,
            index: MultiSigUTXO.vout,
            nonWitnessUtxo:  Buffer.from(MultiSigUTXO.hex, 'hex'),
            redeemScript: Buffer.from(MultiSigUTXO.redeemScript, 'hex')
        })
        .addOutput({
            address: this._serverAddress,
            value: needSendAmount,
        })
        .addOutput({
            address: this._clientAddress,
            value: changeAmount,
        })

    // 嵌入信息
    if (data) {
        // hash chain
        this._data = CryptoJS.SHA256(this._data + CryptoJS.SHA256(data).toString()).toString()
        // console.log('\033[91m', 'client data: ' + this._data, '\033[0m')
        const dataBuffer = Buffer.from(this._data, 'utf8')
        const embed = bitcoin.payments.embed({data: [dataBuffer]})
        this.paymentTx.addOutput({
            script: embed.output,
            value: 0,
        })
    }

    // 支付人签名
    this.paymentTx.signInput(0, this._clientKeyPair)
    this.paymentTx.validateSignaturesOfInput(0, this._clientPubKey)
    return this.paymentTx.toHex()
}

Client.prototype.getMultiSigUTXO = function() {
    return {
        redeemScript: this._redeemScript,
        hex: this.commitmentTx.extractTransaction().toHex(),
        txid: this.commitmentTx.extractTransaction().getId(),
        vout: 0,
        amount: this.commitmentTx.txOutputs[0].value
    }
}

module.exports = Client
