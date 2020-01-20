# oasis-edge-sdk

绿洲边缘SDK是运行在边缘节点的应用，用户可根据边缘节点的硬件形态，去支持接收BLE、ZigBee、LoRa等协议报文数据，配合边缘节点的基础功能，可以将接收数据发送给特定的边缘应用处理，也可通过绿洲云平台管理接入设备的注册认证与配置下发等功能。

# 使用方法

进入build目录,依次执行如下指令：<br>
cmake ..<br>
make<br>
docker build -t oasis-sdk:v1.0 .<br>

命令执行后可在bin目录下查看到生成的可执行文件：edge_app_demo<br>
可通过sudo docker images查看生成的oasis-sdk:v1.0的镜像。


