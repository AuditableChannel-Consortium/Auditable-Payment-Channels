# 安装

## 下载

* wget https://bitcoincore.org/bin/bitcoin-core-0.21.1/bitcoin-0.21.1-x86_64-linux-gnu.tar.gz
* tar -zxvf bitcoin-0.21.1-x86_64-linux-gnu.tar.gz
* mv bitcoin-0.21.1 bitcoin

* ln -s bitcoin/bin/bitcoind /usr/local/bin/bitcoind
* ln -s bitcoin/bin/bitcoin-cli /usr/local/bin/bitcoin-cli

## 配置

* vim .bitcoin/bitcoin.conf
```
server=1
txindex=1
fallbackfee=0.00001
regtest=1

[regtest]
rpcbind=0.0.0.0:18443
rpcuser=root
rpcpassword=123456
rpcallowip=0.0.0.0/0
rpcport=18443
```

## 命令

* bitcoind -daemon # 后台运行
* bitcoin-cli stop # 停止
* bitcoin-cli generatetoaddress nblocks "address" # 挖矿
* or bitcoin-cli -generate 1 # 挖矿
* bitcoin-cli sendtoaddress "address" amount # 转账

* bitcoin-cli help # 帮助
  * createrawtransaction # 创建裸交易
  * signrawtransactionwithwallet # 签名裸交易
  * decoderawtransaction # 解码裸交易
  * sendrawtransaction # 广播裸交易
  * getrawtransaction # 获取裸交易
  * gettransaction # 获取交易
  * decodescript # 解码脚本

## 别名（可选）

* vim .bashrc
```
alias bcli='bitcoin-cli -regtest'
```
