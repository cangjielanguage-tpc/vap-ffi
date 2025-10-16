<div align="center">
<h1>vap-ffi</h1>
</div>

<p align="center">
<img alt="" src="https://img.shields.io/badge/release-v0.0.1-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/project-open-brightgreen" style="display: inline-block;" />
</p>

## 介绍

VAP是在数字娱乐和在线互动的时代，视觉效果的精美程度直接影响用户体验。运用特殊算法打造的强大动画粒子特效渲染组件。它不仅能够为应用程序提供令人惊叹的动画效果，更为用户创造了一种身临其境的视觉享受。同时支持 iOS,Android,Web 平台。

### 特性

- 🚀 支持视频播放、暂停、停止、视频对齐方式

## 软件架构

### 架构图

<p align="center">
<!-- <img src="./doc/assets/framework.png" width="60%" > -->
</p>

### 源码目录

```shell
|-- vap-ffi 
|   |-- entry  # cangjie页面代码
|       |-- src  
|           |-- main 
|               |-- cangjie  # cangjie代码
|                   |-- general_animation.cj       # 测试普通动画
|                   |-- index.cj                  # 主页面
|                   |-- new_on.cj                  # 测试新监听
|                   |-- vapx_animation.cj          # 测试融合动画
|   |-- vap_cj  # cangjie逻辑代码和C/c++交互
|       |-- libs    # vap_cpp编译后的文件
|       |-- src  
|           |-- main  
|               |-- cangjie            # cangjie逻辑代码
|                   |-- data_declare.cj # 公共枚举类
|                   |-- http_req.cj     # http类
|                   |-- index.cj       # C、c++交互类
|                   |-- vap_controller.cj # xcomponent组件控制类
|   |-- vap_cpp  # C/c++代码
|       |-- build    # C/c++代码编译后文件
|       |-- src  
|           |-- main  
|               |-- cpp            # C/c++代码
|                   |-- include    # C/c++代码头文件
|                       |-- mask    # C/c++代码头文件
|                           |-- mask_config.h # 绘画配置文件
|                           |-- mask_render.h # 绘画配置文件
|                           |-- mask_shader.h # 绘画配置文件
|                       |-- mix    # C/c++代码头文件
|                           |-- frame.h         # 绘画文件
|                           |-- mix_render.h    # 绘画文件
|                           |-- mix_shader.h    # 绘画文件
|                           |-- src.h           # 绘画文件
|                       |-- render    # C/c++代码头文件
|                           |-- anim_config.h         # 绘画文件
|                           |-- egl_core.h            # 绘画主文件
|                           |-- plugin_render_cangjie_ffi.h       # C/c++代码和仓颉交互的主逻辑代码
|                           |-- yuv_render.h           # 绘画文件
|                   |-- manager    # C/c++代码头文件
|                       |-- plugin_manager.cpp # C/c++代码主逻辑
|                   |-- mask    # C/c++代码绘画交互类
|                   |-- mix     # C/c++代码绘画交互类
|                   |-- render  # C/c++代码实现类
|                   |-- util  # C/c++代码工具类
|  |---- README.MD  # 安装使用方法
```

### 接口说明

#####  播放

```
vapffi.play();
```

##### 暂停

```
vapffi.pause();
```

##### 停止

```
vapffi.stop();
```

##### 动画循环播放

```
vapffi.setLoop();
```

##### 动画播放速率

```
vapffi.setSpeed();
```

##### 动画播放类型

```
vapffi.getVideoInfo();
```

##### 动画播放对齐方式

```
vapffi.setVideoMode();
```

##### 动画播放区域点击事件

```
vapffi.on();
```

##### 网络路径动画

```
vapffi.setFitType();
```

## 使用说明

### 编译构建

用devecostudio打开项目,然后点击run按钮直接运行app即可自动编译成功

### 把vap作为编译启动
```
用户如何启动vapffi，具体步骤如下：
1. 通过git命令,把自己需要的vap分支拉下来,比如
   git clone -b 分支名  https://gitcode.com/Cangjie-TPC/vap-ffi.git
2. 对vap_cpp中通过build->make Module 'vapcpp'进行编译
3. 将编译后build->defaut->intermediates->cmake->defaut->obj下面的arm64-v8a和x86_64下面的所有文件复制到vap_cj->libs下面，然后直接起服务
```

### 功能示例

在UI中配置XComponent控件

```cj
    XComponent(id: 'xcomponentId_vap', `type`: XComponentType.TEXTURE, controller: vapController)
    .width(100.percent)
    .height(100.percent)

```
播放

```cj
this.vapController.play(uri);
```

暂停

```cj
this.vapController.pause();
```

停止

```cj
this.vapController.stop();
```


## 约束与限制

支持视频格式:
- .mp4 .m3u8 .mov

在下述版本验证通过：

IDE : DevEco Studio 5.1.1.823
Cangjie Compiler: 1.0.1 (cjnative)


## 开源协议 License

本项目基于 [Apache License Version 2.0](/LICENSE) ，请自由的享受和参与开源。

## 参与贡献

欢迎给我们提交PR，欢迎给我们提交Issue，欢迎参与任何形式的贡献。