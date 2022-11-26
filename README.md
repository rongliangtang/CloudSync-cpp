## 项目简介
此项目为深圳大学计算机前沿技术课程的论文复现代码

## 复现论文简介
[Cloud Object Storage Synchronization: Design, Analysis, and Implementation（TPDS）](https://ieeexplore.ieee.org/abstract/document/9802905)

不同计算终端之间的云存储服务在企业和个人用户中得到了大规模应用。它使用户能够实时维护同一份数据副本，减轻了用户繁琐而容易出错的数据管理负担。但是，现在的云存储服务往往是封闭的。用户只能固定使用某个云存储提供商，当衡量性能、成本、安全等因素时，很难从一个云存储提供商转移到另一个云存储提供商。

​本次课程的论文复现工作是，实现支持云存储服务的同步系统，支持实时、多终端的云存储服务之间的同步，并在论文的基础上进行一些改进。

## 开发环境
- 编程环境：MacOS、Clang、CMake、C++17
- 外部库：阿里云OSS SDK、USCiLab/cereal、nlohmann/json、cryptopp

## 编译方法
1. 安装好对应的cmake和编译器（gcc、clang均可）
2. 外部库除了阿里云OSS SDK其他都已在external文件夹中
3. 下载阿里云OSS SDK，在cmakelists.txt中修改相应代码链接上动态库（阿里云官方文档很详细）
4. 用cmake编译即可，就可以获得二进制可执行文件

## 使用方法
修改configuration.json配置文件
在命令行中运行编译获得的二进制可执行文件

例如下面这条命令，前面是可执行文件，后面是配置文件的路径
```
./build/cloudsync ./configuration2.json
```

## 程序演示
待上传......


