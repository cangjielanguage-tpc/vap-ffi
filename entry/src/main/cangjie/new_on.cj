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
    }

    public func cbFn(): Unit {
        this.showVideo = false
    }

    private func play(): Unit {
        try {
            this.vapController.setFitType(FitType.CENTER_CROP)
            this.vapController.setVideoMode(VideoMode.VIDEO_MODE_SPLIT_HORIZONTAL)
            var rootPath = globalAbilityContext.getOrThrow().resourceDir
            var filePath = "${rootPath}/" + videoName
            let opts: ArrayList<CangjieMixData> = ArrayList<CangjieMixData>();
            this.vapController.play(filePath, opts, cbFn)
        } catch (e: Exception) {
            Hilog.error(0, "=======VAPXAnimation  pause error=========", "${e.message}")
        }
    }

    private func stop(): Unit {
        try {
            this.vapController.stop()
        } catch (e: Exception) {
            Hilog.error(0, "=======GeneralAnimation  stop =========", "${e.message}")
        }
    }

    private func pause(): Unit {
        try {
            this.vapController.pause()
        } catch (e: Exception) {
            Hilog.error(0, "=======GeneralAnimation  pause =========", "${e.message}")
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

                    Row(){
                        Button("Skip")
                        .fontWeight(FontWeight.Normal)
                        .onClick(
                            {
                                =>
                                this.stop()
                                this.showVideo = false
                            }
                        )
                        .borderRadius(4)
                        .height(5.percent)
                        .width(20.percent)
                    }.width(100.percent).margin(top:15.px, right: 10.px, bottom: 15.px, left: 10.px)
                    .justifyContent(FlexAlign.SpaceAround).alignItems(VerticalAlign.Bottom)
                }
            }.width(100.percent).height(100.percent).backgroundImage(src: @r(app.media.bg), repeat: ImageRepeat.XY)
            .backgroundImageSize(ImageSize.FILL)
        }
    }
}
