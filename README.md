# 支付通道

## 安装
[bitcoin安装](./README_install.md)

[bitcoinjs文档](https://bitcoinjs-guide.bitcoin-studio.com/bitcoinjs-guide/v5/)

## 账户

### 导入alice和bob

[bitcoin-test-wallets-generator](https://github.com/bitcoin-studio/bitcoin-test-wallets-generator.git)

```
npx bitcointestwalletsgenerator

or

git clone git@github.com:bitcoin-studio/bitcoin-test-wallets-generator.git
node generate_wallets.js
```

### 转账alice

alice: bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g

sendto alice 50BTC 方便后续实验...

* bitcoin-cli -generate 100
* bitcoin-cli sendtoaddress bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g 50
* bitcoin-cli listunspent null null '["bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g"]'

## 流程

支付人：alice
收款人：bob

1. client.createCommitment() => alice创建支付通道

2. client.createRefund() => alice创建退款交易

3. server.signRefund.js => bob签名退款交易

4. broadcast commitment => alice广播支付通道

5. client.incrementPayment() => alice创建一笔支付交易

6. server.checkAndSignPayment() => bob检查并且签名支付交易

7. broadcast payment tx => 情况一：bob广播支付交易

8. broadcast refund tx => 情况二：alice广播退款交易

## [思考](https://zhuanlan.zhihu.com/p/60721681)

问题：如何避免 B 跑路，A 的钱永远锁死在公共账号里面？

答案：双方签名退款交易，确认无误后，alice才会发送保证金到支付通道。

## 注意

广播退款交易，可能发生错误：

```
"error": {
    "code": -26,
    "message": "non-final"
}
```

### [原因](https://www.bookstack.cn/read/MasterBitcoin2CN/spilt.5.ch07.md)

为了杜绝矿工说谎，加强时间安全性，在相对时间锁的基础上又新增了一个BIP。
这是BIP-113，它定义了一个称为“中位时间过去 /（Median-Time-Past）”的新的共识测量机制。
通过取最后11个块的时间戳并计算其中位数作为“中位时间过去”的值。
这个中间时间值就变成了共识时间，并被用于所有的时间计算。
过去约两个小时的中间点，任何一个块的时间戳的影响减小了。
通过这个方法，没有一个矿工可以利用时间戳从具有尚未成熟的时间段的交易中获取非法矿工费。

### [解决](https://bitcoinjs-guide.bitcoin-studio.com/bitcoinjs-guide/v5/part-three-pay-to-script-hash/timelocks/cltv_p2sh.html)
 
* bitcoin-cli getblockchaininfo | grep mediantime
* generatetoaddress 20 bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g
