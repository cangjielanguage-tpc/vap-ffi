/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package ohos_app_cangjie_entry

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
import std.collection.HashMap
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
import std.collection.Map

public class GlobalLoops {
    public static var stateComponent: HashMap<String, Bool> = HashMap<String, Bool>();
}

@Entry
@Component
public class GeneralAnimation {
    @State
    var vapController: VapController = VapController("GeneralAnimation")

    public var map: Map<String, Int64> = HashMap<String, Int64>()

    @State
    var fitType: FitType = FitType.FIT_CENTER

    @State
    var buttonEnabled: Bool = true;

    @State
    var text: String = "fix_center"

    var videoName: String = "great_gift.mp4"
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
        // 组件销毁前取消播放完成轮询器，避免页面销毁后 animator 空转与每帧 FFI 调用
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
            var filePath = "${rootPath}/" + videoName
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
        try {
            this.vapController.pause()
        } catch (e: Exception) {
            Hilog.error(0, "=======VAPXAnimation  pause error=========", "${e.message}")
        }
    }

    private func stop(): Unit {
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

    }

    func download(data: DownLoadData): Unit {
        Hilog.info(0, "=======download  DownLoadData =========", "-----download  DownLoadData-")
    }
}