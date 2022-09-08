# iothub SDK for Linux C
这是一个专门为Linux写的 Iothub MQTT SDK.
## 参考文档
https://www.eclipse.org/paho/index.php?page=clients/c/index.php
## 依赖
本SDK依赖用下面命令安装(Ubuntu或者Debian上):
```sh
sudo apt install make jq -y
```
## SDK 生成
首先上iothub生成设备基础信息，复制进 `device-config.json` 这个文件里面,然后执行:
```
./gen-sdk.sh
```

执行完以后,响应的参数会被配置进文件里面，而应用只需要修改main里面的代码即可.
## 编译
```bash
make clean
make
```
## 设计思路
1. 所有错误结果均不为0, 可能是-1,1,2,3等;
2. 0是唯一正确结果