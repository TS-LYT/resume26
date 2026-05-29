# Linux 环境编程

## 常见内容

```text
argc/argv        命令行参数
getenv/setenv    环境变量
getcwd/chdir     当前工作目录
access           判断文件是否存在或有权限
perror/errno     错误信息
```

## 面试口述

Linux 应用程序启动时可以通过命令行参数和环境变量接收配置。系统调用失败时通常返回 `-1`，并设置 `errno`，工程里要及时检查错误码。

## 运行示例

```bash
APP_MODE=debug ./build/env_demo arg1 arg2
```
