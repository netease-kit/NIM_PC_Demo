# NetEase IM PC Demo

网易云信 PC IM Demo 是基于[网易云信 PC SDK](https://yunxin.163.com/im-sdk-demo) 制作的即时通讯示例程序，UI 库使用 [NIM Duilib](https://github.com/netease-im/NIM_Duilib_Framework) 制作

![GitHub](https://img.shields.io/badge/license-MIT-green.svg)

## sdk版本更新
[传输门](https://dev.yunxin.163.com/docs/product/IM%E5%8D%B3%E6%97%B6%E9%80%9A%E8%AE%AF/%E6%9B%B4%E6%96%B0%E6%97%A5%E5%BF%97/Windows%E7%AB%AF%E6%9B%B4%E6%96%B0%E6%97%A5%E5%BF%97)

## demo版本更新
[传输门](https://dev.yunxin.163.com/docs/product/IM%E5%8D%B3%E6%97%B6%E9%80%9A%E8%AE%AF/Demo%E6%9B%B4%E6%96%B0%E6%97%A5%E5%BF%97/Windows-Demo%E6%9B%B4%E6%96%B0%E6%97%A5%E5%BF%97)

## 预览

![preview](PREVIEW.png)

## 开始

克隆项目到你的磁盘中

```bash
git clone https://github.com/netease-im/NIM_PC_Demo.git
```

全局搜索DEMO_GLOBAL_APP_KEY，std::string strKey =，kAppKey替换成自己的appkey
进入 `NIM_PC_Demo/nim_win_demo` 目录，使用 Visual Studio 2013 Update 5 以上版本 IDE 打开 `nim.sln`，按下 F7 即可编译项目

如您需要编译 64 位工程，请将 `bin/x64_dlls` 目录下的文件复制并替换现有 `bin` 目录下的文件，同时也需要解压 `libs/x64` 目录下的 `cryptlib.zip`。

## 目录

├─`app_sdk` 针对 Demo 应用实现的一些工具类  
├─`bin` 程序输出目录，包含预设的云信 SDK、皮肤、语言文件以及 CEF 依赖  
├─`chatroom` 聊天室的 UI 及业务逻辑实现  
├─`libs` 静态库编译后的输出目录，包含预设的一些 CEF 组件静态库  
├─`nim_sdk` 云信 PC SDK 目录头文件和 C++ 封装层  
├─`third_party` 第三方库，目前仅有 cef_control 有依赖  
├─`toolkits`       
│  ├─`base` 基础类库  
│  ├─`cef` CEF 控件支持  
│  ├─`db` 数据库管理  
│  ├─`duilib` duilib 核心代码，依赖 base 但不依赖 shared  
│  ├─`net` HTTP 库相关头文件  
│  ├─`shared` 包含一些工具的封装，如命令行解析、日志、路径处理等  
│  ├─`ui_components` 基于 duilib 封装的常用组件库如 `msgbox`、`toast`、`cef_control` 等  

## 交流

 - 遇到问题：关注[云信开发人员手册](https://dev.yunxin.163.com/)帮助您理解和使用云信 SDK 接口
 - 提交缺陷：在确保使用最新版本依然存在问题时请尽量以简洁的语言描述清楚复现该问题的步骤并提交 issue
 - 功能建议：如果你有什么好的想法或者提案，欢迎提交 issue 与我们交流