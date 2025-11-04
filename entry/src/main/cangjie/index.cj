/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
package ohos_app_cangjie_entry

internal import ohos.base.*
internal import ohos.component.Row
internal import ohos.component.Button
internal import ohos.component.Text
internal import ohos.component.CustomView
internal import ohos.component.CJEntry
internal import ohos.component.loadNativeView
internal import ohos.state_manage.*
internal import ohos.component.If
internal import ohos.component.Stack
internal import ohos.component.Flex
internal import ohos.component.FlexParams
internal import ohos.component.FlexDirection
internal import ohos.component.FlexAlign
internal import ohos.component.ItemAlign
internal import ohos.base.LengthProp
internal import ohos.base.Color
internal import ohos.state_manage.SubscriberManager
internal import ohos.state_manage.ObservedProperty
internal import ohos.state_manage.LocalStorage
internal import ohos.component.Column
import ohos.state_macro_manage.Entry
import ohos.state_macro_manage.Component
import ohos.router.Router
import ohos.component.Scroll
import ohos.component.*

foreign func CangjiePluginRenderInit(str: CString): Unit

@Entry
@Component
class EntryView {
    func build() {
        Column() {
            Scroll() {
                Flex(
                    FlexParams(direction: FlexDirection.Column, alignItems: ItemAlign.Center,
                        justifyContent: FlexAlign.Center)) {
                    Flex(
                        FlexParams(direction: FlexDirection.Column, alignItems: ItemAlign.Center,
                            justifyContent: FlexAlign.Center)) {
                        Button("测试普通动画")
                            .onClick({
                                ent => Router.push(url: "GeneralAnimation")
                            })
                            .margin(top: 10.percent, bottom: 10.percent)
                    }.width(100.percent).height(15.percent)

                    Flex(
                        FlexParams(direction: FlexDirection.Column, alignItems: ItemAlign.Center,
                            justifyContent: FlexAlign.Center)) {
                        Button("测试融合动画")
                            .onClick({
                                ent => Router.push(url: "VAPXAnimation")
                            })
                            .margin(top: 10.percent, bottom: 10.percent)
                    }.width(100.percent).height(15.percent)

                    Flex(
                        FlexParams(direction: FlexDirection.Column, alignItems: ItemAlign.Center,
                            justifyContent: FlexAlign.Center)) {
                        Button("测试新监听").onClick({
                            ent => Router.push(url: "NewOn")
                        }).margin(top: 10.percent, bottom: 10.percent)
                    }.width(100.percent).height(15.percent)
                }.margin(10.percent).padding(20).borderRadius(10).backgroundColor(0xF5DEB3)
            }.width(100.percent).height(100.percent)
        }
    }
}
