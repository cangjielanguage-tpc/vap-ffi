<div align="center">
<h1>vap-ffi</h1>
</div>

<p align="center">
<img alt="" src="https://img.shields.io/badge/release-v1.0.1-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/build-pass-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjc-v1.0.3-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjcov-92.0-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/project-open-brightgreen" style="display: inline-block;" />
</p>

## 介绍

vap-ffi是在数字娱乐和在线互动的时代，视觉效果的精美程度直接影响用户体验。运用特殊算法打造的强大动画粒子特效渲染组件。它不仅能够为应用程序提供令人惊叹的动画效果，更为用户创造了一种身临其境的视觉享受。

### 特性

- 🚀 支持视频播放、暂停、停止、视频对齐方式

## 软件架构

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
|  |---- README.MD  # 安装使用方法
```

### 接口说明

主要类和函数接口说明详见 [API](doc/feature_api.md)

## 使用说明

### 编译构建

用devecostudio打开项目,然后点击run按钮直接运行app即可自动编译成功

### 把vap作为编译启动

1.用户如何启动vapffi，具体步骤如下：
```
1. 通过git命令,把自己需要的vap分支拉下来,比如
   git clone -b 分支名  https://gitcode.com/Cangjie-TPC/vap-ffi.git
2. 对vap_cpp中通过build->make Module 'vapcpp'进行编译
3. 将编译后build->defaut->intermediates->cmake->defaut->obj下面的arm64-v8a和x86_64下面的所有文件复制到vap_cj->libs下面，然后直接起服务
```

2.修改自身应用 entry/src/main/cangjie 下的 cjpm.toml 文件，在 [dependencies] 字段下添加:
```toml
[dependencies]
  [dependencies.vap_cj]
    path = "${vap_cj}"
```

3.在项目中使用 import highlightguide.* 引用 highlightguide 项目
```cangjie
import vap_cj.*
```

### 功能示例

```
internal import ohos.base.*
internal import ohos.component.Column
internal import ohos.component.Row
internal import ohos.component.Button
internal import ohos.component.Text
internal import ohos.component.CustomView
internal import ohos.component.CJEntry
internal import ohos.component.loadNativeView
internal import ohos.state_manage.SubscriberManager
internal import ohos.state_manage.ObservedProperty
internal import ohos.state_manage.LocalStorage
internal import ohos.hilog.Hilog
internal import ohos.resource_manager.*
import ohos.component.SelectOption
import ohos.component.Stack
import ohos.component.Scroll
import ohos.component.Select
import ohos.component.Visibility
import ohos.state_macro_manage.r
import ohos.state_manage.*
import ohos.state_macro_manage.Component
import ohos.state_macro_manage.Entry
import ohos.state_macro_manage.State
import std.collection.ArrayList
import ohos.component.ImageRepeat
import ohos.component.ViewBase
import ohos.component.ImageSize
import ohos.component.ScrollDirection
import ohos.component.BorderStyle
import ohos.component.FontWeight
import vap_cj.*
import ohos.component.XComponent
import ohos.component.XComponentType

@Entry
@Component
public class GeneralAnimation {
    @State
    var vapController: VapController = VapController("GeneralAnimation")

    @State
    var fitType: FitType = FitType.FIT_CENTER

    @State
    var buttonEnabled: Bool = true;

    @State
    var text: String = "fix_center"

    func viewInit() {
        println("GeneralAnimation")
    }
    @State
    var values: Array<SelectOption> =
        [
            SelectOption("fix_xy", icon: ""),
            SelectOption("fix_center", icon: ""),
            SelectOption("center_crop", icon: "")
        ]

    protected func aboutToAppear() {
    	this.vapController.stop()
    }
    protected func onPageHide() {
    	this.vapController.stop()
    }
    protected func onPageShow() {}



    func build() {
        Column() {
            Stack {
                XComponent(id: 'vap_xcomponentId_General', `type`: XComponentType.SURFACE, controller: vapController).width(100.percent).height(100.percent)
                .backgroundColor(Color.TRANSPARENT).visibility(getVisible(this.buttonEnabled))
            }.width(100.percent).height(90.percent).backgroundImage(src: @r(app.media.bg), repeat: ImageRepeat.XY).backgroundImageSize(ImageSize.FILL)

            Scroll() {
                Row() {
                    Select(this.values)
                        .selected(1)
                        .value(text)
                        .onSelect(
                            {
                                index: Int32, event =>
                                this.fitType = this.fitType.getFitType(index)
                                Hilog.info(0, "=======  GeneralAnimation Select =========", "[${index}]")
                            }
                        )
                    Button("Play")
                        .fontWeight(FontWeight.Normal)
                        .width(40.percent)
                        .margin(top: 15.px, right: 10.px, bottom: 15.px, left: 10.px)
                        .onClick({
                            event => this.play()
                        }).enabled(this.buttonEnabled)

                    Button("Pause")
                        .width(20.percent)
                        .margin(top: 15.px, right: 10.px, bottom: 15.px, left: 10.px)
                        .onClick({
                            event => this.pause()
                        })

                    Button("Stop")
                        .width(20.percent)
                        .margin(top: 15.px, right: 10.px, bottom: 15.px, left: 10.px)
                        .onClick({
                            event => this.stop()
                        })
                }
            }
                .width(100.percent)
                .scrollable(ScrollDirection.Horizontal)
                .padding(bottom: 10.px)
                .margin(left: 5.px, right: 5.px)
                .border(width: 4.px, color: Color.GRAY, radius: 10.px, style: BorderStyle.Solid)
                .backgroundColor(Color.WHITE)
        }.width(100.percent).height(100.percent)
    }

    private func play(): Unit {
        try {
            var rootPath = globalAbilityContext.getOrThrow().resourceDir
            var filePath = "${rootPath}/great_gift.mp4"
            let opts: ArrayList<CangjieMixData> = ArrayList<CangjieMixData>();
            this.vapController.setFitType(this.fitType)
            this.vapController.setVideoMode(VideoMode.VIDEO_MODE_SPLIT_HORIZONTAL)

            this.vapController.setDownloadAttribute(50000, download)
            this.vapController.setLoop(1)
            this.buttonEnabled = false
            this.vapController.play(filePath, opts, cbFn)
        } catch (e: Exception) {
            Hilog.error(0, "=======GeneralAnimation  play error=========", "${e.message}")
        }
    }

    private func pause(): Unit {
        Hilog.info(0, "=======VAPXAnimation  pause =========", "pause")
        try {
            this.vapController.pause()
        } catch (e: Exception) {
            Hilog.error(0, "=======VAPXAnimation  pause error=========", "${e.message}")
        }
    }

    private func stop(): Unit {
        Hilog.info(0, "=======VAPXAnimation  stop =========", "stop")
        try {
            this.vapController.stop()
        } catch (e: Exception) {
            Hilog.error(0, "=======VAPXAnimation  stop error=========", "${e.message}")
        }
    }

    func getVisible(b: Bool): Visibility {
        if (b) {
            return Visibility.Hidden
        } else {
            return Visibility.Visible
        }
    }

    func cbFn(): Unit {
        this.buttonEnabled = true
        Hilog.info(0, "=======GeneralAnimation  cbFn =========", "-----GeneralAnimation--cbFn-")
        // 触发应用侧回调
    }

    func download(data: DownLoadData): Unit {
        Hilog.info(0, "=======download  DownLoadData =========", "-----download  DownLoadData-")
    }
}
```


## 约束与限制

支持视频格式:
- .mp4 .m3u8 .mov

在下述版本验证通过：

    DevEco Studio 5.1.1 Release(5.1.1.840)


## 开源协议 License

本项目基于MIT License。详细信息,请参见[License](/LICENSE)。

## 参与贡献

欢迎给我们提交PR，欢迎给我们提交Issue，欢迎参与任何形式的贡献。