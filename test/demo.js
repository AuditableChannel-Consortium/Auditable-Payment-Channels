const fs = require('fs')
const { alice, bob } = require('../wallets')
const payment_channel = require('../')
const regtestUtils = new payment_channel.RegtestUtils()

// 创建支付人
const client = new payment_channel.Client(alice[1].wif)

// 创建收款人
const server = new payment_channel.Server(bob[1].wif)

// 获取UTXO集合
const utxos = regtestUtils.listUnspent(client._clientAddress)

// alice创建支付通道
const commitmentHex = client.createCommitment({
    serverPubKey: server._serverPubKey, // 收款人公钥
    utxos: utxos, // UTXO集合
    commitmentAmount: 10, // 支付通道数量
    txFee: 0.00001 // 手续费
})

// alice创建退款交易
const refundHex = client.createRefund()

// bob签名退款交易
const refundSignHex = server.signRefund(refundHex)

// alice广播支付通道
const commitmentTxid = regtestUtils.broadcast(commitmentHex)
console.log('commitment txid:', commitmentTxid)

// 支付流程：alice创建一笔支付交易，bob检查并且签名支付交易
let paymentHex = ''
let paymentSignHex = ''
function pay(i, amount) {
    paymentHex = client.incrementPayment(amount, `this is the ${i} payment.`)
    paymentSignHex = server.checkAndSignPayment(paymentHex, amount)
}

pay(1, 1) // 第一笔交易：1 BTC
pay(2, 3) // 第二笔交易：3 BTC
pay(3, 5) // 第三笔交易：5 BTC
// ...

// 情况一：bob广播支付交易
const paymentTxid = regtestUtils.broadcast(paymentSignHex)
console.log('payment txid:', paymentTxid)

// 情况二：alice广播退款交易
// const refundTxid = regtestUtils.broadcast(refundSignHex)
// console.log('refund txid:', refundTxid)

const result = {
    'commitment': commitmentHex,
    'commitment.txid': commitmentTxid,
    'refund.unsigned': refundHex,
    'refund.signed': refundSignHex,
    'payment.unsigned': paymentHex,
    'payment.signed': paymentSignHex,
    'payment.txid': paymentTxid,
    'embed.data': client._data
}
fs.writeFileSync('output/demo.log', JSON.stringify(result, null, 2))
