# ONVIF 协议详解

## 1. ONVIF 是什么

ONVIF 是网络摄像机、NVR、门禁等安防设备常用的开放协议规范。它不是单一传输协议，而是一套基于 Web Service 的设备管理接口。

常见能力：

```text
设备发现
获取设备信息
获取媒体配置
获取 RTSP 地址
云台 PTZ 控制
事件订阅
时间同步
用户认证
```

## 2. ONVIF 和 RTSP 的关系

很多摄像机同时支持 ONVIF 和 RTSP：

```text
ONVIF：负责发现设备、登录认证、获取媒体配置、获取 RTSP URL
RTSP ：拿到 URL 后负责拉流控制
RTP  ：实际传输音视频数据
```

面试可以这样说：

```text
ONVIF 偏设备管理和能力查询，RTSP 偏音视频流控制。
```

## 3. ONVIF 设备发现

ONVIF 设备发现通常使用 WS-Discovery：

```text
协议：UDP
组播地址：239.255.255.250
端口：3702
报文格式：SOAP XML
```

客户端发送 Probe：

```text
客户端 -> 239.255.255.250:3702
SOAP Probe 请求
```

设备收到后返回 ProbeMatch，里面通常包含设备服务地址 `XAddrs`。

## 4. SOAP 请求是什么

ONVIF 请求本质上是 HTTP POST + XML：

```text
POST /onvif/device_service HTTP/1.1
Content-Type: application/soap+xml

<s:Envelope>
  <s:Body>
    <tds:GetDeviceInformation/>
  </s:Body>
</s:Envelope>
```

## 5. 认证

很多 ONVIF 接口需要用户名密码。常见方式是 WS-Security UsernameToken，里面包含：

```text
Username
PasswordDigest
Nonce
Created
```

真实项目不要手写完整 ONVIF 栈，建议使用 gSOAP、pugixml/tinyxml2 或成熟 ONVIF SDK。

## 6. 面试口述

ONVIF 是安防设备的管理协议，基于 SOAP/XML 和 HTTP。发现设备时用 WS-Discovery UDP 组播，拿到设备服务地址后，再通过 HTTP SOAP 调用接口，比如获取设备信息、媒体配置和 RTSP 地址。RTSP 则负责后续拉流。

## 7. 运行发现示例

```bash
./build/onvif_probe_demo
```

如果局域网里有 ONVIF 摄像机，可能会收到 ProbeMatch 响应。没有设备时会超时退出。
