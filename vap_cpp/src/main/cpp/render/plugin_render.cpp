/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include <cstdint>
#include "plugin_render.h"
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include "napi/n_func_arg.h"
#include "napi/n_val.h"
#include <string>
//#include <js_native_api.h>
//#include <js_native_api_types.h>
#include <log.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <uv.h>
#include <optional>

std::unordered_map<int64_t, std::shared_ptr<PluginRender>> PluginRender::m_instance;
OH_NativeXComponent_Callback PluginRender::m_callback;

PluginRender::PluginRender(int64_t &id)
{
    this->eglCore_ = new EGLCore();
    hasDraw_ = 0;
    hasChangeColor_ = 0;
    this->m_id = id;
}

std::shared_ptr<PluginRender> PluginRender::GetInstance(int64_t &id)
{
    if(m_instance.find(id) == m_instance.end()){
        std::shared_ptr<PluginRender> instance = std::make_shared<PluginRender>(id);
        m_instance[id] = instance;
        return instance;
    }else{
        return m_instance[id];
    }
}

std::string RemoveSurroundingChars(const std::string& str, char startChar,char endChar){
    size_t start = str.find_first_not_of(startChar);
    if(start == std::string::npos){
        return "";
    }
    size_t end = str.find_last_not_of(endChar);
    if(end == std::string::npos){
        return "";
    }
    return str.substr(start,end - start + 1);
}

static std::optional<OH_Drawing_TextAlign> intToTextAlign(int value){
    switch (value){
        case static_cast<int>(OH_Drawing_TextAlign::TEXT_ALIGN_CENTER):
            return OH_Drawing_TextAlign::TEXT_ALIGN_CENTER;
        case static_cast<int>(OH_Drawing_TextAlign::TEXT_ALIGN_JUSTIFY):
            return OH_Drawing_TextAlign::TEXT_ALIGN_JUSTIFY;
        case static_cast<int>(OH_Drawing_TextAlign::TEXT_ALIGN_START):
            return OH_Drawing_TextAlign::TEXT_ALIGN_START;
        case static_cast<int>(OH_Drawing_TextAlign::TEXT_ALIGN_END):
            return OH_Drawing_TextAlign::TEXT_ALIGN_END;
        case static_cast<int>(OH_Drawing_TextAlign::TEXT_ALIGN_LEFT):
            return OH_Drawing_TextAlign::TEXT_ALIGN_LEFT;
        case static_cast<int>(OH_Drawing_TextAlign::TEXT_ALIGN_RIGHT):
            return OH_Drawing_TextAlign::TEXT_ALIGN_RIGHT;
        default:
            return std::nullopt;
    }
}

//void PluginRender::Release(int64_t &id){
//    std::shared_ptr<PluginRender> render = PluginRender::GetInstance(id);
//    if(nullptr != render){
//        render->surfaceDestroyed_ = true;
//    }
//}

void PluginRender::ChangeColor(){
    eglCore_->ChangeColor(hasChangeColor_);
}

//void PluginRender::InitNativeWindow(OHNativeWindow *window){
//    eglCore_->EglContextInit(window);
//}

void PluginRender::DrawPattern(){
    eglCore_->Draw(hasDraw_);
}

void PluginRender::UpdateNativeWindowSize(int width, int height){
    eglCore_->UpdateSize(width, height);
    if(!hasChangeColor_ && !hasDraw_){
        eglCore_->Background();
    }
}

int32_t PluginRender::HasDraw(){
    return hasDraw_;
}

int32_t PluginRender::HasChangedColor(){
    return hasChangeColor_;
}

static std::optional<OH_Drawing_FontWeight> intToFontWeight(int value){
    switch (value){
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_100):
            return OH_Drawing_FontWeight::FONT_WEIGHT_100;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_200):
            return OH_Drawing_FontWeight::FONT_WEIGHT_200;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_300):
            return OH_Drawing_FontWeight::FONT_WEIGHT_300;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_400):
            return OH_Drawing_FontWeight::FONT_WEIGHT_400;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_500):
            return OH_Drawing_FontWeight::FONT_WEIGHT_500;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_600):
            return OH_Drawing_FontWeight::FONT_WEIGHT_600;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_700):
            return OH_Drawing_FontWeight::FONT_WEIGHT_700;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_800):
            return OH_Drawing_FontWeight::FONT_WEIGHT_800;
        case static_cast<int>(OH_Drawing_FontWeight::FONT_WEIGHT_900):
            return OH_Drawing_FontWeight::FONT_WEIGHT_900;
        default:
            return std::nullopt;
    }
}

static void ParseMixParamTag(NVal &nValOneOpt, std::string &tag)
{
    if (nValOneOpt.HasProp(PROP_TAG)) {
        auto [succ, resData, length] = nValOneOpt.GetProp(PROP_TAG).ToUTF8String();
        if (succ) {
            LOGD("parse mix tag %{public}s", resData.get());
            tag = resData.get();
            tag = "[" + tag + "]";
        } else {
            LOGE("parse mix tag not set");
            return;
        }
    }
}

static void ParseMixParamTxt(NVal &nValOneOpt, MixInputData &mixInputData)
{
    if (nValOneOpt.HasProp(PROP_TXT)) {
        LOGD("parse mix txt");
        auto [txtSucc, resData, length] = nValOneOpt.GetProp(PROP_TXT).ToUTF8String();
        if (txtSucc) {
            mixInputData.txt = resData.get();
            LOGD("parse mix txt %{public}s", resData.get());
        }
    }
}

static void ParseMixParamImg(NVal &nValOneOpt, MixInputData &mixInputData)
{
    if (nValOneOpt.HasProp(PROP_IMAGE)) {
        LOGD("parse mix img");
        auto [imgSucc, resData, length] = nValOneOpt.GetProp(PROP_IMAGE).ToUTF8String();
        if (imgSucc) {
            mixInputData.imgUri = resData.get();
        }
    }
}

static void ParseMixParamColor(NVal &nValOneOpt, MixInputData &mixInputData)
{
    if (nValOneOpt.HasProp(PROP_COLOR)) {
        auto colorNVal = nValOneOpt.GetProp(PROP_COLOR);
        if (colorNVal) {
            auto [succA, a] = colorNVal.GetProp("a").ToUint32();
            auto [succR, r] = colorNVal.GetProp("r").ToUint32();
            auto [succG, g] = colorNVal.GetProp("g").ToUint32();
            auto [succB, b] = colorNVal.GetProp("b").ToUint32();
            
            if (succA && succB && succR && succG) {
                mixInputData.color = {a, r, g, b};
                mixInputData.isSet |= SET_COLOR;
                LOGD("parse mix color %{public}x argb: %{public}02x-%{public}02x-%{public}02x-%{public}02x",
                    mixInputData.isSet, a, r, g, b);
            }
        }
    }
}

static void ParseMixParamFontWeight(NVal &nValOneOpt, MixInputData &mixInputData)
{
    if (nValOneOpt.HasProp(PROP_FONT_WEIGHT)) {
        auto [succ, resData] = nValOneOpt.GetProp(PROP_FONT_WEIGHT).ToInt32();
        if (succ) {
            auto fontWeight = intToFontWeight(resData);
            if (fontWeight) {
                mixInputData.fontWeight = fontWeight.value();
                mixInputData.isSet |= SET_FONT_WEIGHT;
                LOGD("parse mix fontWeight %{public}x fw: %{public}d", mixInputData.isSet, resData);
            }
        }
    }
}

static void ParseMixParamTextAlign(NVal &nValOneOpt, MixInputData &mixInputData)
{
    if (nValOneOpt.HasProp(PROP_TEXT_ALIGN)) {
        auto [succ, resData] = nValOneOpt.GetProp(PROP_TEXT_ALIGN).ToInt32();
        if (succ) {
            auto textAlign = intToTextAlign(resData);
            if (textAlign) {
                mixInputData.textAlign = textAlign.value();
                mixInputData.isSet |= SET_TEXT_ALIGN;
                LOGD("parse mix textAlign %{public}x ta: %{public}d", mixInputData.isSet, resData);
            }
        }
    }
}

static void ParseMixParam(std::map<std::string, MixInputData> &mixData, napi_env env, NFuncArg &funcArg)
{
    napi_value v2 = funcArg.GetArg(NARG_POS::SECOND);
    if (!v2) {
        LOGE("not get second param");
        return;
    }
    
    NVal nVal2(env, v2);
    auto [succ1, isArray] = nVal2.IsArray();
    if (!succ1 || !isArray) {
        LOGE("the second param is not array");
        return;
    }
    uint32_t len = ZERO;
    napi_get_array_length(env, v2, &len);
    if (len == ZERO) {
        LOGE("the array is empty");
        return;
    }
    
    for (uint32_t i = 0; i < len; i++) {
        napi_value element;
        napi_get_element(env, v2, i, &element);
        NVal nValOneOpt(env, element);
        MixInputData mixInputData;
        
        std::string tag;
        ParseMixParamTag(nValOneOpt, tag);
        ParseMixParamTxt(nValOneOpt, mixInputData);
        ParseMixParamImg(nValOneOpt, mixInputData);
        ParseMixParamColor(nValOneOpt, mixInputData);
        ParseMixParamFontWeight(nValOneOpt, mixInputData);
        ParseMixParamTextAlign(nValOneOpt, mixInputData);
        
        LOGD("parse mix data tag%{public}s ta: %{public}d fw: %{public}d", tag.c_str(),
            mixInputData.textAlign, mixInputData.fontWeight);
        mixData.insert(std::make_pair(tag, mixInputData));
    }
}

static void CreateAnimConfig(napi_env env, JSAnimConfig &jsAnimConfig, NVal &animConfig)
{
    auto alphaRect = NVal::CreateObject(env);
    auto x = NVal::CreateInt64(env, jsAnimConfig.alphaPointRect.x);
    auto y = NVal::CreateInt64(env, jsAnimConfig.alphaPointRect.y);
    auto w = NVal::CreateInt64(env, jsAnimConfig.alphaPointRect.w);
    auto h = NVal::CreateInt64(env, jsAnimConfig.alphaPointRect.h);
    alphaRect.AddProp("x", x.val_);
    alphaRect.AddProp("y", y.val_);
    alphaRect.AddProp("w", w.val_);
    alphaRect.AddProp("h", h.val_);
    
    auto rgbRect = NVal::CreateObject(env);
    auto x1 = NVal::CreateInt64(env, jsAnimConfig.rgbPointRect.x);
    auto y1 = NVal::CreateInt64(env, jsAnimConfig.rgbPointRect.y);
    auto w1 = NVal::CreateInt64(env, jsAnimConfig.rgbPointRect.w);
    auto h1 = NVal::CreateInt64(env, jsAnimConfig.rgbPointRect.h);
    rgbRect.AddProp("x", x1.val_);
    rgbRect.AddProp("y", y1.val_);
    rgbRect.AddProp("w", w1.val_);
    rgbRect.AddProp("h", h1.val_);
    
    auto version = NVal::CreateInt64(env, jsAnimConfig.version);
    auto totalFrames = NVal::CreateInt64(env, jsAnimConfig.totalFrames);
    auto width = NVal::CreateInt64(env, jsAnimConfig.width);
    auto height = NVal::CreateInt64(env, jsAnimConfig.height);
    auto videoWidth = NVal::CreateInt64(env, jsAnimConfig.videoWidth);
    auto videoHeight = NVal::CreateInt64(env, jsAnimConfig.videoHeight);
    auto orien = NVal::CreateInt64(env, jsAnimConfig.orien);
    auto fps = NVal::CreateInt64(env, jsAnimConfig.fps);
    auto isMix = NVal::CreateBool(env, jsAnimConfig.isMix);
    auto currentFrame = NVal::CreateInt64(env, jsAnimConfig.currentFrame);
    
    animConfig.AddProp("version", version.val_);
    animConfig.AddProp("totalFrames", totalFrames.val_);
    animConfig.AddProp("width", width.val_);
    animConfig.AddProp("height", height.val_);
    animConfig.AddProp("videoWidth", videoWidth.val_);
    animConfig.AddProp("videoHeight", videoHeight.val_);
    animConfig.AddProp("orien", orien.val_);
    animConfig.AddProp("fps", fps.val_);
    animConfig.AddProp("isMix", isMix.val_);
    animConfig.AddProp("alphaPointRect", alphaRect.val_);
    animConfig.AddProp("rgbPointRect", rgbRect.val_);
    animConfig.AddProp("currentFrame", currentFrame.val_);
}

