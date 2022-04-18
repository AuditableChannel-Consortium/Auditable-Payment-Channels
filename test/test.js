const fs = require('fs')
const {alice, bob} = require('../wallets')
const payment_channel = require('../')

// 测试参数：UTXOS
const UTXOS = [
    {
        "txid": "920256ffe4bd3e42d23bb89136ac5d065f4c3e811b762735f5fc7c7d518b1c6f",
        "vout": 0,
        "scriptPubKey": "0014fb8820f35effa054399540b8ca86040d8ddaa4d5",
        "amount": 1001.00000000
    },
    {
        "txid": "705db39de487157bb2305dcbb22834277627de243517cd73ca68b0dc8060b9c6",
        "vout": 0,
        "scriptPubKey": "0014fb8820f35effa054399540b8ca86040d8ddaa4d5",
        "amount": 12.50001000
    },
    {
        "txid": "765d40f9c889fa52c056be287873546d82aef707a5a87ef79d674a2452fefbf4",
        "vout": 0,
        "scriptPubKey": "0014fb8820f35effa054399540b8ca86040d8ddaa4d5",
        "amount": 12.50001000
    }
]
// 测试参数：输入数量
const UTXOS_COUNTS = [1, 2, 3]
// 测试参数：交易次数
const TX_TIMES = [1, 10, 100]
// 测试参数：测试次数（多次测试取平均值）
const TEST_TIMES = 10

function test(UTXOS, TX_TIME) {
    // 创建支付人
    const client = new payment_channel.Client(alice[1].wif)
    // 创建收款人
    const server = new payment_channel.Server(bob[1].wif)
    // 手续费
    const txFee = 0.00001
    // 每次交易 1 BTC
    const amount = 1

    // alice创建支付通道
    const commitmentStart = new Date().getTime()
    const commitmentHex = client.createCommitment({
        serverPubKey: server._serverPubKey, // 收款人公钥
        utxos: UTXOS, // UTXO集合
        commitmentAmount: (amount + txFee) * TX_TIME, // 支付通道数量（预付手续费）
        txFee: txFee // 手续费
    })
    const commitmentEnd = new Date().getTime()

    // alice创建退款交易
    const refundStart = new Date().getTime()
    const refundHex = client.createRefund()
    const refundEnd = new Date().getTime()

    // bob签名退款交易
    const refundSignStart = new Date().getTime()
    const refundSignHex = server.signRefund(refundHex)
    const refundSignEnd = new Date().getTime()

    // 交易流程
    let paymentTime = 0
    let paymentSignTime = 0
    let paymentHexLength = 0
    let paymentSignHexLength = 0
    // 交易次数动态变化
    for (let i = 1; i <= TX_TIME; i++) {
        // alice创建一笔支付交易
        const paymentStart = new Date().getTime()
        const paymentHex = client.incrementPayment(amount, `this is the ${i} payment.`)
        const paymentEnd = new Date().getTime()
        paymentTime += paymentEnd - paymentStart
        paymentHexLength += paymentHex.length
        // bob检查并且签名支付交易
        const paymentSignStart = new Date().getTime()
        const paymentSignHex = server.checkAndSignPayment(paymentHex, amount)
        const paymentSignEnd = new Date().getTime()
        paymentSignTime += paymentSignEnd - paymentSignStart
        paymentSignHexLength += paymentSignHex.length
    }
    return {
        "alice创建支付通道": {
            "size": commitmentHex.length,
            "time": commitmentEnd - commitmentStart
        },
        "alice创建退款交易": {
            "size": refundHex.length,
            "time": refundEnd - refundStart
        },
        "bob签名退款交易": {
            "size": refundSignHex.length,
            "time": refundSignEnd - refundSignStart
        },
        "alice创建一笔支付交易": {
            "size": paymentHexLength / TX_TIME,
            "time": paymentTime / TX_TIME
        },
        "bob检查并且签名支付交易": {
            "size": paymentSignHexLength / TX_TIME,
            "time": paymentSignTime / TX_TIME
        }
    }
}

// 遍历参数
const result = {}
UTXOS_COUNTS.forEach(function (UTXOS_COUNT) {
    const _UTXOS = UTXOS.slice(0, UTXOS_COUNT)
    TX_TIMES.forEach(function (TX_TIME) {
        const key = `输入数量=${UTXOS_COUNT}/交易次数=${TX_TIME}`
        console.log(key)
        // 初始化
        let commitmentHexLength = 0
        let commitmentTime = 0
        let refundHexLength = 0
        let refundTime = 0
        let refundSignHexLength = 0
        let refundSignTime = 0
        let paymentHexLength = 0
        let paymentTime = 0
        let paymentSignHexLength = 0
        let paymentSignTime = 0
        // 多次测试取平均值
        for(let i = 0; i < TEST_TIMES; i++) {
            const result = test(_UTXOS, TX_TIME)
            commitmentHexLength += result['alice创建支付通道'].size
            commitmentTime += result['alice创建支付通道'].time
            refundHexLength += result['alice创建退款交易'].size
            refundTime += result['alice创建退款交易'].time
            refundSignHexLength += result['bob签名退款交易'].size
            refundSignTime += result['bob签名退款交易'].time
            paymentHexLength += result['alice创建一笔支付交易'].size
            paymentTime += result['alice创建一笔支付交易'].time
            paymentSignHexLength += result['bob检查并且签名支付交易'].size
            paymentSignTime += result['bob检查并且签名支付交易'].time
        }
        result[key] = {
            "alice创建支付通道": {
                "size": commitmentHexLength / TEST_TIMES,
                "time": commitmentTime / TEST_TIMES,
            },
            "alice创建退款交易": {
                "size": refundHexLength / TEST_TIMES,
                "time": refundTime / TEST_TIMES,
            },
            "bob签名退款交易": {
                "size": refundSignHexLength / TEST_TIMES,
                "time": refundSignTime / TEST_TIMES,
            },
            "alice创建一笔支付交易": {
                "size": paymentHexLength / TEST_TIMES,
                "time": paymentTime / TEST_TIMES,
            },
            "bob检查并且签名支付交易": {
                "size": paymentSignHexLength / TEST_TIMES,
                "time": paymentSignTime / TEST_TIMES,
            }
        }
    })
})
fs.writeFileSync('output/test.log', JSON.stringify(result, null, 2))
