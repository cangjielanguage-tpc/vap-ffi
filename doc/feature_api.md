# vap-ffi 特性文档

## 功能介绍
    vap-ffi是在数字娱乐和在线互动的时代，视觉效果的精美程度直接影响用户体验。运用特殊算法打造的强大动画粒子特效渲染组件。
    它不仅能够为应用程序提供令人惊叹的动画效果，更为用户创造了一种身临其境的视觉享受。

## 1 接口类

```cangjie
public class VapController {
    
    /*
     * 动画播放
     * 
     * 参数 uri - 动画路径
     * 参数 mixData - 入参(目前融合动画有数据)
     * 参数 cbFn - 回调函数，用于关闭播放窗口
     *
     * 返回值 Unit
     */
    public func play(uri: String, mixData: ArrayList<CangjieMixData>, cbFn: () -> Unit): Unit
    
    /*
     * 动画暂停
     *
     * 返回值 Unit
     */
    public func pause(): Unit
    
    /*
     * 动画停止
     *
     * 返回值 Unit
     */
    public func stop(): Unit
    
    /*
     * 设置播放完成标志（写入 native 层原子标志）
     *
     * 说明：配合 UI 侧 Animator 帧回调轮询使用。native 播放结束回调（非 UI 线程）
     * 只置位标志，UI 主线程帧回调轮询取回后执行完成回调，保证 @State 等状态更新
     * 始终发生在主线程。调用 play() 前应使用 setPlayDone(false) 清除历史标志。
     *
     * 参数 value - 播放完成标志
     *
     * 返回值 Unit
     */
    public func setPlayDone(value: Bool): Unit
    
    /*
     * 查询播放完成标志（读取后自动清零）
     *
     * 说明：仅供 UI 主线程的帧回调轮询调用。
     *
     * 返回值 Bool
     */
    public func isPlayDone(): Bool
    
    /*
     * 动画循环播放
     *
     * 参数 loops - 循环次数(建议循环次数不宜过多，防止程序内存开销过大)
     *
     * 返回值 Unit
     */
    public func setLoop(loops: Int64): Unit
    
    /*
     * 视频对齐方式（支持FIT_XY,FIT_CENTER,CENTER_CROP）
     *
     * 参数 fitType - 对齐方式
     *
     * 返回值 Unit
     */
    public func setFitType(fitType: FitType): Unit
    
    /*
     * 兼容老视频
     *
     * 参数 videoMode - 兼容方式
     *
     * 返回值 Unit
     */
    public func setVideoMode(videoMode: VideoMode): Unit
    
    /*
     * 获取兼容老视频
     *
     * 参数 that - 兼容老视频类型
     *
     * 返回值 Int64
     */
    public func getVideoMode(that: VideoMode): Int64
    
    /*
     * 获取视频对齐方式
     *
     * 参数 that - 对齐方式
     *
     * 返回值 Int64
     */
    public func getFitType(that: FitType): Int64
}
```

```cangjie
/*
 * 视频对齐方式枚举类
 *
 */
public enum FitType {
    
    /*
     * 拉伸适配尺寸
     */
    FIT_XY
    
    /*
     * 保持比例居中显示
     */
    FIT_CENTER
    
    /*
     * 保持比例居中裁剪
     */
    CENTER_CROP

    /*
     * 数字转化为枚举类视频对齐方式
     *
     * 参数 that - 对齐方式数字
     *
     * 返回值 Int64
     */
    public func getFitType(that: Int32): FitType
    
    /*
     * 视频对齐方式转化为数字
     *
     * 参数 that - 对齐方式
     *
     * 返回值 Int64
     */
    public func getFitType(that: FitType): Int32
}
```

```cangjie
/*
 * 视频对齐方式 (兼容老版本视频模式)
 */
public enum VideoMode {
    /*
     * 视频左右对齐（alpha左\rgb右）
     */
    VIDEO_MODE_SPLIT_HORIZONTAL
    
    /*
     * 视频上下对齐（alpha上\rgb下）
     */
    VIDEO_MODE_SPLIT_VERTICAL
    
    /*
     * 视频左右对齐（rgb左\alpha右）
     */
    VIDEO_MODE_SPLIT_HORIZONTAL_REVERSE
    
    /*
     * 视频上下对齐（rgb上\alpha下）
     */
    VIDEO_MODE_SPLIT_VERTICAL_REVERSE
}
```

```cangjie
/*
 * 动画参数类
 */
@C
public struct CangjieMixData {
    
    /*
     * 动画参数构造方法
     *
     * 参数 tag - 标记
     * 参数 txt - 文本信息
     * 参数 imgUri - 对齐方式
     * 参数 colorA - 动画颜色a
     * 参数 colorR - 动画颜色r
     * 参数 colorG - 动画颜色g
     * 参数 colorB - 动画颜色b
     *
     */
    public init(tag: String, txt: String, imgUri: String, colorA: Int64, colorR: Int64, colorG: Int64, colorB: Int64)
    
    /*
     * 动画参数构造方法
     *
     * 参数 tag - 标记
     * 参数 txt - 文本信息
     * 参数 imgUri - 视频路径字符串类型
     * 参数 colorA - 动画颜色a
     * 参数 colorR - 动画颜色r
     * 参数 colorG - 动画颜色g
     * 参数 colorB - 动画颜色b
     * 参数 vapFontWeight - 字体粗细属性
     * 参数 vapTextAlign - 文本水平对齐方式
     *
     */
    public init(tag: String, txt: String, imgUri: String, colorA: Int64, colorR: Int64, colorG: Int64, colorB: Int64,
        vapFontWeight: Int32, vapTextAlign: Int32)
    
    /*
     * 动画参数构造方法
     *
     * 参数 tag - 标记
     * 参数 txt - 文本信息
     * 参数 imgUri - 视频路径字符串类型
     *
     */
    public init(tag: String, txt: String, imgUri: String)
    
    /*
     * 视频对齐方式转化为数字
     *
     * 参数 str - 设置CString类型的视频路径
     *
     * 返回值 Unit
     */
    public mut func setImgUri(str: CString):Unit
}
```

## 2 示例

```cangjie
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
import kit.UIKit.AnimatorResult
import kit.UIKit.AnimatorOptions
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

    // 播放完成轮询器：UI 动画管线帧回调（主线程）驱动，轮询 native 原子标志
    var playDonePoller: ?AnimatorResult = None

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
    protected func aboutToDisappear() {
        // 组件销毁前取消播放完成轮询器，避免页面销毁后 animator 空转
        this.playDonePoller?.cancel()
        this.playDonePoller = None
    }



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
            this.vapController.setPlayDone(false)
            this.buttonEnabled = false
            this.vapController.play(filePath, opts, cbFn)
            this.startPlayDonePoller()
        } catch (e: Exception) {
            Hilog.error(0, "=======GeneralAnimation  play error=========", "${e.message}")
        }
    }

    // 启动播放完成轮询：onFrame 由 UI 动画管线在主线程回调（约每帧一次），
    // 检测到 native 播放完成标志后取消轮询并执行完成回调（cbFn 内更新 @State 合法）
    private func startPlayDonePoller(): Unit {
        this.playDonePoller?.cancel()
        this.playDonePoller = AnimatorResult(
            AnimatorOptions(
                duration: 1000,
                easing: "linear",
                iterations: -1,
                begin: 0.0,
                end: 1.0
            )
        )
        this.playDonePoller?.onFrame = {
            value: Float64 =>
            if (this.vapController.isPlayDone()) {
                this.playDonePoller?.cancel()
                this.playDonePoller = None
                this.cbFn()
            }
        }
        this.playDonePoller?.play()
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

























