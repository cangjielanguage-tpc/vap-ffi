/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package ohos_app_cangjie_entry

internal import ohos.component.loadNativeView
internal import ohos.state_manage.SubscriberManager
internal import ohos.state_manage.ObservedProperty
internal import ohos.state_manage.LocalStorage
internal import ohos.hilog.Hilog
internal import ohos.base.*
import vap_cj.*
import ohos.state_macro_manage.*
import ohos.router.*
import ohos.component.*
import std.collection.ArrayList

@Entry
@Component
class NewOn {
    @State
    var vapController: VapController = VapController("NewOn")
    @State
    var fitType: FitType = FitType.CENTER_CROP
    @State
    var showVideo: Bool = true
    var videoName: String = "demo.mp4";

    var vapState: VapState = VapState.UNKNOWN

    protected func aboutToAppear() {
        this.vapController.setcallback(cbFn)
        var rootPath = globalAbilityContext.getOrThrow().resourceDir
        var filePath = "${rootPath}/" + videoName
        this.vapController.setUrl(filePath)
    }

    func viewInit() {
        println("NewOn")
    }

    public func cbFn(): Unit {
        this.showVideo = false
    }

    private func play(): Unit {
        try {
            this.on()
            this.vapController.setFitType(FitType.CENTER_CROP)
            this.vapController.setVideoMode(VideoMode.VIDEO_MODE_SPLIT_HORIZONTAL)
            var rootPath = globalAbilityContext.getOrThrow().resourceDir
            var filePath = "${rootPath}/" + videoName
            let opts: ArrayList<CangjieMixData> = ArrayList<CangjieMixData>();
            this.vapController.play(filePath, opts, cbFn)
        } catch (e: Exception) {
            Hilog.error(0, "VAPXAnimation  pause error", "VAPXAnimation  pause error message： ${e.message}")
        }
    }

    private func stop(): Unit {
        try {
            this.vapController.stop()
        } catch (e: Exception) {
            Hilog.error(0, "GeneralAnimation  stop error message", "GeneralAnimation  stop error message：${e.message}")
        }
    }

    private func pause(): Unit {
        try {
            this.vapController.pause()
        } catch (e: Exception) {
            Hilog.error(0, "GeneralAnimation pause error message", "error message： ${e.message}")
        }
    }

    private func on(): Unit {
        try {
            this.vapController.on("stateChange", callbackOn)
        } catch (e: Exception) {
            Hilog.error(0, "GeneralAnimation  pause  error message", " error message： ${e.message}")
        }
    }

    func callbackOn(state: Int64, ret: Int64): Unit {
        var retOpt: Option<Any> = ret
        if(state == this.vapState.getVapState(VapState.DESTROY)){
            this.showVideo = false;
        }
    }

    func build() {
        Column() {
            Stack() {
                if (!this.showVideo) {
                    Column() {
                        Button("返回").onClick({
                            event => Router.back()
                        })
                        Text("进入主页")
                        Text("占位文本")
                            .width(100.percent)
                            .textAlign(TextAlign.Center)
                            .border(width: 1.px)
                            .padding(10)
                            .fontColor(Color.BLUE)
                            .textOverflow(TextOverflow.Ellipsis)
                    }.width(100.percent).height(100.percent)
                } else {
                    XComponent(id: 'vap_xcomponentId_newOn', `type`: XComponentType.SURFACE, controller: vapController)
                    .width(100.percent).height(90.percent).backgroundColor(Color.TRANSPARENT)
                }
            }.width(100.percent).height(100.percent).backgroundImage(src: @r(app.media.bg), repeat: ImageRepeat.XY)
            .backgroundImageSize(ImageSize.FILL)
        }
    }
}
