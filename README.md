# transgate

```
 _____   _____        ___   __   _   _____   _____       ___   _____   _____  
|_   _| |  _  \      /   | |  \ | | /  ___/ /  ___|     /   | |_   _| | ____| 
  | |   | |_| |     / /| | |   \| | | |___  | |        / /| |   | |   | |__   
  | |   |  _  /    / / | | | |\   | \___  \ | |  _    / / | |   | |   |  __|  
  | |   | | \ \   / /  | | | | \  |  ___| | | |_| |  / /  | |   | |   | |___  
  |_|   |_|  \_\ /_/   |_| |_|  \_| /_____/ \_____/ /_/   |_|   |_|   |_____| 
```

transgate是一个运行于Linux平台上的Web服务器，它由C++ 14编写，目前支持处理HTTP/1.1的静态GET请求以及FastCGI请求，支持通过配置文件配置多站点、FastCGI、最长空闲时间等功能。

## 特点

1. 高性能。采用多线程异步I/O多路复用模型，请求处理速度较快。
2. 低占用，低泄露。内存采用智能指针管理，理论上无内存泄露。同时内存已进行合理的预分配，减少了不必要的内存占用。
3. 配置简易。使用JSON格式单配置文件，附带初始配置文档，简单需求下不需要更改过多的配置项。
4. 运行简单。直接在后台执行即可提供持续的Web服务。
5. 稳定性高。

## 特性

1. HTTP请求剖析器(src/http/http_parser.*)；
2. 支持处理普通GET请求；
3. 支持处理FastCGI请求(如php-fpm)；
4. HTTP 1.1部分特性，包括长连接及最长无活动连接时间限制；

## 安装和运行

安装前，你需要确保你的机器已经正确配置了cmake 3.0以上的版本，你可在软件源中下载并安装cmake。

``` bash
# Ubuntu
sudo apt-get install cmake
# Arch Linux
sudo pacman -S cmake
```

安装之前，你可能需要保证你的Linux内核已经完整支持了`SO_REUSEPORT`特性。然后通过`git`或其他方法下载源代码。之后进入源代码目录，然后执行以下命令完成安装：

``` bash
mkdir build
cd build
cmake ..
make
sudo mkdir /etc/transgate
sudo cp ../transgate.json /etc/transgate
```

之后你可拷贝`build`目录下的`transgate`文件并在任何地方执行它。

## 配置文件说明

项目根目录下已具有一示例性配置文件`transgate.json`，文件中各个配置项的意义如下：

## 性能测试

### 测试环境

```
CPU: Core i7 6700HQ
内存：8 GiB
网卡：Realtek 1000M + Intel(R) Dual Band Wireless-AC 3165
操作系统：Arch Linux
测试项目：WebBench v1.5
```

### 测试结果

