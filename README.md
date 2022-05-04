# Auditable Payment Channel

## Install

[bitcoin install](./README_install.md)

[bitcoinjs file](https://bitcoinjs-guide.bitcoin-studio.com/bitcoinjs-guide/v5/)

## Account

### Load Users Alice and Bob

[bitcoin-test-wallets-generator](https://github.com/bitcoin-studio/bitcoin-test-wallets-generator.git)

```
npx bitcointestwalletsgenerator

or

git clone git@github.com:bitcoin-studio/bitcoin-test-wallets-generator.git
node generate_wallets.js
```

### Create Account

if your wallet name is zzz

* bitcoin-cli createwallet zzz
* bitcoin-cli loadwallet zzz

### Transfer to Alice

Alice: bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g

sendto Alice 50BTC 

* bitcoin-cli -generate 100
* bitcoin-cli sendtoaddress bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g 50
* bitcoin-cli listunspent null null '["bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g"]'

## Run
* npm install
* node test/demo.js
* node test/test.js
* node test/demo_aduitable.js
* node test/test_aduitable.js

## Process

The Sender： Alice
The Receiver： Bob

1. client.createCommitment() => Alice 
   Create Payment Channel 

2. client.createRefund() => Alice
   Create refund transaction 

3. server.signRefund.js => Bob
   Sign refund transaction 

4. broadcast commitment => Alice
   Broadcast created channel

5. client.incrementPayment() => Alice
   Generate a commitment transaction

6. server.checkAndSignPayment() => Bob
   Verify and sign commitment transaction

7. broadcast payment tx => 

(Condition.1) Bob broadcasts commitment transaction to Bitcoin network

8. broadcast refund tx => 

(Condition.2) Alice broadcasts refund transaction to redeem her deposit

## Remark

There may exist error when broadcasting refund transaction

```
"error": {
    "code": -26,
    "message": "non-final"
}
```

### [Solution](https://bitcoinjs-guide.bitcoin-studio.com/bitcoinjs-guide/v5/part-three-pay-to-script-hash/timelocks/cltv_p2sh.html)

* bitcoin-cli getblockchaininfo | grep mediantime
* generatetoaddress 20 bcrt1qlwyzpu67l7s9gwv4gzuv4psypkxa4fx4ggs05g

