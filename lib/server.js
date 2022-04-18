const bitcoin = require('bitcoinjs-lib')
const BTC = 100000000

function Server(serverWif, network) {
    this._network = network ? network : bitcoin.networks.regtest
    this._serverKeyPair = bitcoin.ECPair.fromWIF(serverWif, this._network)
    this._serverPubKey = this._serverKeyPair.publicKey
    this._serverAddress = bitcoin.payments.p2wpkh({ pubkey: this._serverPubKey, network: this._network }).address

    // 已经收款的数量
    this._receivedAmount = 0
}

Server.prototype.signRefund = function (refundHex) {
    const refundTx = bitcoin.Psbt.fromHex(refundHex, {network: this._network})

    // TODO check refund tx（暂时忽略）

    // 收款人签名
    refundTx.signInput(0, this._serverKeyPair)
    refundTx.validateSignaturesOfInput(0, this._serverPubKey)
    refundTx.finalizeAllInputs()
    return refundTx.extractTransaction().toHex()
}

Server.prototype.checkAndSignPayment = function (paymentHex, amount) {
    // 未签名的支付交易
    this.paymentTx = bitcoin.Psbt.fromHex(paymentHex, {network: this._network})

    // 检查是否符合要求
    const needReceivedAmount = this._receivedAmount + Math.round(amount * BTC)
    let paymentAmount = 0
    this.paymentTx.txOutputs.forEach(element => {
        if (element.address === this._serverAddress) {
            paymentAmount = element.value
        }
    })
    if (paymentAmount < needReceivedAmount) {
        throw new Error(`支付通道：本次交易比特币不符合期望数量！ ${paymentAmount} < ${needReceivedAmount}`)
    }
    this._receivedAmount = needReceivedAmount
    console.log('\033[91m', `server receive: ${amount} BTC, allReceive: ${needReceivedAmount / BTC}`, '\033[0m')

    // 收款人签名
    this.paymentTx.signInput(0, this._serverKeyPair)
    this.paymentTx.validateSignaturesOfInput(0, this._serverKeyPair.publicKey)
    this.paymentTx.finalizeAllInputs()
    return this.paymentTx.extractTransaction().toHex()
}

module.exports = Server
