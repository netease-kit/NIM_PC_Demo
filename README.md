# NetEase IM PC Demo

网易云信 PC IM Demo 是基于[网易云信 PC SDK](https://yunxin.163.com/im-sdk-demo) 制作的即时通讯示例程序，UI 库使用 [NIM Duilib](https://github.com/netease-im/NIM_Duilib_Framework) 制作

![GitHub](https://img.shields.io/badge/license-MIT-green.svg) | [![Actions Status](https://github.com/netease-im/NIM_PC_Demo/workflows/MSBuild/badge.svg)](https://github.com/netease-im/NIM_PC_Demo/actions)

## 预览

![preview](preview.png)

## 最低要求

- CMake 3.19 或以上版本 [Download](https://cmake.org/download/)
- Visual Studio 2017 或以上版本 [Download](https://visualstudio.microsoft.com/zh-hans/downloads/)
- Git [Download](https://git-scm.com/downloads)
- Python3 [Download](https://www.python.org/downloads/)
- Conan >1.65.0 <2.0.0（python -m pip install conan==1.65.0）

## 开发步骤

您需要根据最低要求中的版本安装对应的工具，安装完成后，首先克隆项目到你的磁盘中：

```
git clone https://github.com/netease-im/NIM_PC_Demo.git --depth 10
```

### 输出 Debug 工程

执行如下命令初始化 Debug 调试版本的项目：

> [!NOTE]
> 首次初始化 Debug 或 Release 工程时会自动下载并编译依赖的三方库代码，这个过程可能相对较慢，请耐心等待。当您再次编译时，将不再重复下载和编译三方库代码而是使用上一次编译后的产物。

```bash
# 生成 64 位 Debug 工程，如果您未指定 Visual Studio 版本将默认使用本机安装的最新版本
cmake -Bbuild -Ax64 -DCMAKE_BUILD_TYPE=Debug

# 生成 32 位 Debug 工程
cmake -Bbuild -AWin32 -DCMAKE_BUILD_TYPE=Debug
```

> [!TIP]
> Visual Studio 2017 以下版本工具链不支持使用 cmake -A 参数来指定目标产物架构，您可以使用如下命令分别生成 32 位和 64 位的工程
>
> `cmake -Bbuild -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Debug`
>
> `cmake -Bbuild -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug`

您可以通过打开 build 目录下的 `nim_win_demo.sln` 来进行调试或通过 CMake 命令直接编译：

```bash
cmake --build build --config Debug
```

### 输出 Release 工程

如您需要编译 Release 版本，则将上面的命令中 Debug 修改为 Release 即可：

```bash
# 生成 64 位 Release 工程，
cmake -Bbuild -Ax64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 生成 32 位 Release 工程，
cmake -Bbuild -AWin32 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

> [!TIP]
> 所有产物均生成在项目根目录下的 bin 文件夹中，项目编译完成后您可以直接从 bin 目录下运行 nim_demo.exe 来启动 Demo

> [!IMPORTANT]
> 需要注意的是，由于 Debug 和 Release 版本的依赖库文件都会拷贝到 bin 目录下，因此在切换 Debug 和 Release 版本时请使用 `git clean -xdf` 命令清理 bin 目录下的临时文件

### 制作安装包

当所有产物编译完成后，您可以执行如下命令编译安装包，安装包最终输出在 bin 目录下：

```bash
cmake --build build --config Release --target installer
```

## 更新 IM SDK 版本

IM SDK 当前使用 Conan 包管理工具管理，如果您需要检索已经发布的版本，可通过如下命令搜索：

```bash
conan search nim -r yunxin
```

输出结果类似：

```bash
Existing package recipes:

nim/9.10.0@yunxin/stable
nim/9.12.1@yunxin/stable
nim/9.12.2@yunxin/stable
nim/9.13.0@yunxin/stable
nim/9.14.2@yunxin/stable
nim/9.15.0@yunxin/stable
nim/9.15.1@yunxin/stable
nim/9.18.0@yunxin/stable
nim/10.5.0@yunxin/stable
```

您可以打开 `conanfile.py` 文件，修改 `self.requires("nim/*.*.*@yunxin/stable")` 为指定版本即可，修改完成后重新执行 CMake 初始化命令即可，CMake 会自动下载并编译指定版本的 IM SDK。

## 交流

- 遇到问题：关注[云信开发人员手册](https://dev.yunxin.163.com/)帮助您理解和使用云信 SDK 接口
- 提交缺陷：在确保使用最新版本依然存在问题时请尽量以简洁的语言描述清楚复现该问题的步骤并提交 Issue
- 功能建议：如果你有什么好的想法或者提案，欢迎提交 Issue 与我们交流

#推荐好友使用云信，即享 1500 元京东卡奖励# 点击参与https://yunxin.163.com/promotion/recommend?channel=github
[![main](https://yx-web-nosdn.netease.im/quickhtml%2Fassets%2Fyunxin%2Fdefault%2F2022%2Fgithub%2F233.png)](https://yunxin.163.com/promotion/recommend?channel=github)
