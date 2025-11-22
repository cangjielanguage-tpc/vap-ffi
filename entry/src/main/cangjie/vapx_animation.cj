/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package ohos_app_cangjie_entry

internal import ohos.base.*
internal import ohos.component.Column
internal import ohos.component.Row
internal import ohos.component.Text
internal import ohos.component.CustomView
internal import ohos.component.loadNativeView
internal import ohos.state_manage.SubscriberManager
internal import ohos.state_manage.ObservedProperty
internal import ohos.state_manage.LocalStorage
internal import ohos.hilog.Hilog
internal import std.convert.*
internal import ohos.component.SelectOption
import vap_cj.*
import ohos.component.Stack
import ohos.component.Scroll
import ohos.component.Select
import ohos.component.Visibility
import ohos.state_macro_manage.r
import ohos.state_manage.*
import ohos.state_macro_manage.Component
import ohos.state_macro_manage.Entry
import ohos.state_macro_manage.State
import ohos.component.XComponent
import ohos.component.XComponentType
import std.collection.ArrayList
import ohos.component.ImageRepeat
import ohos.component.ViewBase
import ohos.component.ImageSize
import ohos.component.ScrollDirection
import ohos.component.BorderStyle
import ohos.component.FontWeight
import ohos.component.TextInput
import ohos.component.InputType
import ohos.component.Alignment
import ohos.component.VerticalAlign

@Entry
@Component
public class VAPXAnimation {
    @State
    var buttonEnabled: Bool = true;

    var srcInfo: Array<SrcInfo> = Array<SrcInfo>()

    var srcType: SrcType = SrcType.UNKNOWN
    @State
    var vapController: VapController = VapController("VAPXAnimation")
    @State
    var fitType: FitType = FitType.FIT_CENTER;

    @State
    var textAlign: VAPTextAlign = VAPTextAlign.TEXT_ALIGN_LEFT;

    @State
    var text: String = "fix_center"

    var videoName: String = "demo.mp4";

    @State
    var colorText: String = "red"

    var colorIdx: Int32 = 0

    @State
    var colorValues: Array<SelectOption> =
        [
            SelectOption("red", icon: ""),
            SelectOption("green", icon: ""),
            SelectOption("blue", icon: "")
        ]

    @State
    var weightsValues: Array<SelectOption> =
        [
            SelectOption("thin", icon: ""),
            SelectOption("extra_light", icon: ""),
            SelectOption("light", icon: ""),
            SelectOption("normal", icon: ""),
            SelectOption("medium", icon: ""),
            SelectOption("semi_bold", icon: ""),
            SelectOption("bold", icon: ""),
            SelectOption("extra_bold", icon: ""),
            SelectOption("black", icon: "")
        ]

    @State
    var values: Array<SelectOption> =
        [
            SelectOption("fix_xy", icon: ""),
            SelectOption("fix_center", icon: ""),
            SelectOption("center_crop", icon: "")
        ]

    @State
    var fontAligValues: Array<SelectOption> =
        [
            SelectOption("left", icon: ""),
            SelectOption("right", icon: ""),
            SelectOption("center", icon: ""),
            SelectOption("justify", icon: ""),
            SelectOption("start", icon: ""),
            SelectOption("end", icon: "")
        ]

    @State
    var color: ColorBean = ColorBean();

    @State
    var showColor: Int64 = 0;

    @State
    var fontWeight: VAPFontWeight = VAPFontWeight.NORMAL;

    public func onPageShow() {
        println("onPageShow")
        this.showColor = (((this.color.r & 0xff) << 16) | ((this.color.g & 0xff) << 8) | ((this.color.b & 0xff)))
    }

    public func onPageHide() {
        println("onPageHide")
        this.vapController.stop()
    }

    public func viewInit() {
        println("VAPXAnimation")
    }

    public func build() {
        Column() {
            Stack() {
                 XComponent(id: 'vap_xcomponentId_VAPXAnimation', `type`: XComponentType.SURFACE, controller: vapController).width(100.percent).height(90.percent)
                .backgroundColor(Color.TRANSPARENT).visibility(getVisible(this.buttonEnabled))

                Scroll() {
                    Row() {
                        Text('色彩分量:').fontWeight(FontWeight.Bold)
                        Select(this.colorValues)
                            .selected(0)
                            .value(colorText)
                            .onSelect(
                                {
                                    index: Int32, event =>
                                    this.colorIdx = index
                                }
                            )

                        TextInput()
                            .setType(InputType.Number)
                            .width(80)
                            .maxLength(3)
                            .onChange(
                                {
                                    value: String =>
                                    if(value.isEmpty()){
                                        return;
                                    }
                                    if (this.colorIdx == 0) {
                                        this.color.r = Int64.parse(value) & 0xff
                                    } else if (this.colorIdx == 1) {
                                        this.color.g = Int64.parse(value) & 0xff
                                    } else if (this.colorIdx == 2) {
                                        this.color.b = Int64.parse(value) & 0xff
                                    }
                                    this.showColor = (((this.color.r & 0xff) << 16) | ((this.color.g & 0xff) << 8) |
                                    ((this.color.b & 0xff)))
                                }
                            )

                        Text('当前色彩:').backgroundColor(Color.WHITE).foregroundColor(Color.BLACK).fontWeight(FontWeight.Bold)
                        Text("").width(20).height(20).backgroundColor(this.showColor)

                        Text('视频对齐:').fontWeight(FontWeight.Bold)
                        Select(this.values)
                            .selected(1)
                            .value(text)
                            .onSelect(
                                {
                                    index: Int32, event =>
                                    this.fitType = this.fitType.getFitType(index)
                                }
                            )

                        Text('字体权重:').fontWeight(FontWeight.Bold)
                        Select(this.weightsValues)
                            .selected(3)
                            .value('normal')
                            .onSelect(
                                {
                                    index: Int32, event =>
                                    this.fontWeight = this.fontWeight.getFontWeight(index)
                                }
                            )

                        Text('字体对齐:').fontWeight(FontWeight.Bold)
                        Select(this.fontAligValues)
                            .selected(0)
                            .value('left')
                            .onSelect(
                                {
                                    index: Int32, event =>
                                    this.textAlign = this.textAlign.getVAPTextAlign(index)
                                }
                            )
                    }
                }
                    .width(100.percent)
                    .scrollable(ScrollDirection.Horizontal)
                    .padding(bottom: 10.px)
                    .margin(left: 5.px, right: 5.px)
                    .border(width: 4.px, color: Color.GRAY, radius: 10.px, style: BorderStyle.Solid)
                    .backgroundColor(Color.WHITE)
                    .opacity(0.6)
            }.width(100.percent).height(93.percent).alignContent(Alignment.Bottom)
             .backgroundImage(src: @r(app.media.bg), repeat: ImageRepeat.XY).backgroundImageSize(ImageSize.FILL)

            Row() {
                Button("Play")
                    .fontWeight(FontWeight.Normal)
                    .margin(top: 15.px, right: 10.px, bottom: 15.px, left: 10.px)
                    .width(40.percent)
                    .onClick({
                        event => this.play()
                    })
                    .enabled(this.buttonEnabled)

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
            }.width(100.percent).margin(top: 15.px, right: 10.px, bottom: 15.px, left: 10.px)
             .justifyContent(FlexAlign.SpaceAround).alignItems(VerticalAlign.Bottom)
        }
    }

    private func play(): Unit {
        try {
            var rootPath = globalAbilityContext.getOrThrow().resourceDir
            var filePath = "${rootPath}/" + videoName
            this.srcInfo = this.vapController.getVideoInfo(filePath)
            this.vapController.setFitType(this.fitType)
            this.vapController.setLoop(1)
            let mixDatas: Array<CangjieMixData> = []
            if (!this.srcInfo.isEmpty()) {
                for (i in 0..this.srcInfo.size-1) {
                    if (this.srcInfo[i].srcType == this.srcType.getSrcTypeInt(SrcType.IMG)) {
                        mixDatas[i] = CangjieMixData(this.srcInfo[i].tag, "", "图片地址")
                    } else if (this.srcInfo[i].srcType == this.srcType.getSrcTypeInt(SrcType.TXT)) {
                        mixDatas[i] = CangjieMixData(this.srcInfo[i].tag, '文本信息', "",
                            this.color.a, this.color.r, this.color.g, this.color.b)
                    }
                }
            } else {
                Hilog.info(0, "error not get srcInfo", "error not get srcInfo")
            }
            let opts: ArrayList<CangjieMixData> = ArrayList() //TODO
            if(!this.srcInfo.isEmpty()){
                for (i in 0..this.srcInfo.size-1) {
                    if(this.srcInfo[i].srcType == this.srcType.getSrcTypeInt(SrcType.IMG)){
                        var opt: CangjieMixData = CangjieMixData(this.srcInfo[i].tag, "", filePath + '/head1.png')
                        opts.add(opt)
                    } else if(this.srcInfo[i].srcType == this.srcType.getSrcTypeInt(SrcType.TXT)){
                        var opt: CangjieMixData = CangjieMixData(this.srcInfo[i].tag, "星河Harmony NEXT","",
                            this.color.a, this.color.r, this.color.g, this.color.b, this.fontWeight.getFontWeightF(this.fontWeight),
                            this.textAlign.getVAPTextAlignF(this.textAlign))
                        opts.add(opt)
                    }
                }
            }

            this.buttonEnabled = false
            this.vapController.play(filePath, opts, cbFn) // TODO
        } catch (e: Exception) {
            Hilog.error(0, "=======play error=========", "${e.message}")
        }
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
    }
}

public class ColorBean {
    var a: Int64 = 255;
    var r: Int64 = 2;
    var g: Int64 = 255;
    var b: Int64 = 187;
}
