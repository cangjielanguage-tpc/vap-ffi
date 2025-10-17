/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#include "plugin_render_cangjie_ffi.h"
#include "napi/n_func_arg.h"
#include "plugin_render.h"
#include <cmath>
#include <cstdint>
#include <hilog/log.h>
#include <uv.h>
#include "common/common.h"
#include "plugin_manager.h"
#include "util/cj_string_util.h"
#include "util/log.h"

#ifndef CJ_EXPORT
#ifdef WINDOWS_PLATFORM
#define CJ_EXPORT __declspec(dllexport)
#else
#define CJ_EXPORT __attribute__((visibility("default")))
#endif
#endif

std::unordered_map<std::string, std::shared_ptr<PluginRenderCangjieFFI>> PluginRenderCangjieFFI::m_instance;
OH_NativeXComponent_Callback PluginRenderCangjieFFI::m_callback;

extern "C" { // Cangjie
static void ParseMixParamTag(std::string tag, std::string &tag1) {
    if (!tag.empty()) {
        LOGD("--2222----ParseMixParamTag mix tag %{public}s", tag.c_str());
        tag1 = tag;
    } else {
        LOGE("--2222----parse mix tag not set");
        return;
    }
}

static void ParseMixParamTxt(MixInputData &mixInputData, std::string txt) {
    LOGD("parse mix txt");
    if (!txt.empty()) {
        mixInputData.txt = txt;
        LOGD("--333----ParseMixParamTag mix txt %{public}s", txt.c_str());
    }
}

static void ParseMixParamImg(MixInputData &mixInputData, std::string imgUri) {
    LOGD("parse mix img");
    if (!imgUri.empty()) {
        mixInputData.imgUri = imgUri;
        LOGD("--333----ParseMixParamTag mix txt %{public}s", imgUri.c_str());
    }
}

static void ParseMixParamColor(MixInputData &mixInputData, std::uint64_t valueA, std::uint64_t valueR, 
                               std::uint64_t valueG, std::uint64_t valueB) 
{
     if (valueA && valueR && valueG && valueB) {
        uint32_t valueA32 = static_cast<uint32_t>(valueA);
        uint32_t valueR32 = static_cast<uint32_t>(valueR);
        uint32_t valueG32 = static_cast<uint32_t>(valueG);
        uint32_t valueB32 = static_cast<uint32_t>(valueB);
        mixInputData.color = {valueA32, valueR32, valueG32, valueB32};
        mixInputData.isSet |= SET_COLOR;
     }
}

static std::optional<OH_Drawing_FontWeight> intToFontWeight(int value) {
    switch (value) {
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

static void ParseMixParamFontWeight(MixInputData &mixInputData, int value) {
    auto fontWeight = intToFontWeight(value);
    if (fontWeight) {
        mixInputData.fontWeight = fontWeight.value();
        mixInputData.isSet |= SET_FONT_WEIGHT;
    }
}

static std::optional<OH_Drawing_TextAlign> intToTextAlign(int value) {
    switch (value) {
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

static void ParseMixParamTextAlign(MixInputData &mixInputData, int value) {
    auto textAlign = intToTextAlign(value);
    if (textAlign) {
        mixInputData.textAlign = textAlign.value();
        mixInputData.isSet |= SET_TEXT_ALIGN;
    }
}

void ParseMixParam(std::map<std::string, MixInputData> &mixData, CangjieMixData *mixDataCj, size_t length) {
    LOGD("----12345---start---ParseMixParam----length");
    for (uint32_t i = 0; i < length; i++) {
        // 获取当前结构体元素的引用
        MixInputData mixInputData;
        std::string tag;
        ParseMixParamTag(mixDataCj[i].tag,tag);
        ParseMixParamTxt(mixInputData, mixDataCj[i].txt);
        ParseMixParamImg(mixInputData, mixDataCj[i].imgUri);
        std::uint64_t colorA = mixDataCj[i].colorA;
        std::uint64_t colorB = mixDataCj[i].colorB;
        std::uint64_t colorG = mixDataCj[i].colorG;
        std::uint64_t colorR = mixDataCj[i].colorR;
        ParseMixParamColor(mixInputData, colorA, colorR, colorG, colorB);
        ParseMixParamFontWeight(mixInputData, mixDataCj[i].VAPFontWeight);
        ParseMixParamTextAlign(mixInputData, mixDataCj[i].VAPTextAlign);
        mixData.insert(std::make_pair(tag, mixInputData));
    }
}

static void CreateAnimConfig(napi_env env, JSAnimConfig &jsAnimConfig, NVal &animConfig) {
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

struct CallbackContext4cj {
    void (*callbackRef)() = nullptr;
    int what = 0;
    int arg1 = 0;
    int arg2 = 0;
    char *obj;
    int ijk_id = 0;
};


CJBytesArray RemoveSurroundingChars(const std::string& str, char startChar, char endChar)
{
    size_t start = str.find_first_not_of(startChar);
    if (start == std::string::npos) {
        return {0, nullptr}; // 如果字符串中没有找到非startChar的字符，返回空字符串
    }
    size_t end = str.find_last_not_of(endChar);
    if (end == std::string::npos) {
        return {0, nullptr}; // 如果字符串中没有找到非endChar的字符，返回空字符串
    }
    str.substr(start, end - start + 1);
    return pag::stringToCJBytesArray((char*)str.c_str());
}

static std::vector<SrcInfoo> GenSrcInfo(std::map<std::string, Src> &src)
{
    std::vector<SrcInfoo> srcInfos;
    size_t i = 0;
    for (auto ele: src) {
        auto srcId = atoi(ele.first.c_str());
        CJBytesArray tagDeal = RemoveSurroundingChars(ele.second.srcTag, '[', ']');
        auto type = static_cast<int>(ele.second.srcType);
        SrcInfoo srcInfo = SrcInfoo();
        srcInfo.srcId = srcId;
        srcInfo.tag = tagDeal;
        srcInfo.srcType = type;
        srcInfos.push_back(srcInfo);
        i++;
    }
    return srcInfos;
}

void freeCJBytesArray(CJBytesArray *cjbytesArray, int64_t i);
void freeCJBytesArray(CJBytesArray *cjbytesArray, int64_t i){
    if(cjbytesArray[i].buf == nullptr){
        return;
    }
    uint8_t *buf = cjbytesArray[i].buf;
    if(buf != nullptr){
        free(buf);
    }
}

void Callback(void *asyncContext) {
    LOGI("--------Callback start");
    if (asyncContext == nullptr) {
        return;
    }
    uv_loop_t *loop = nullptr;
    CallbackContext *context = (CallbackContext *)asyncContext;
    uv_work_t *work = new uv_work_t;
    work->data = context;
    if (work == nullptr) {
        LOGI("napi-->work null");
        return;
    }
    loop = uv_default_loop();
    uv_queue_work(
        loop, work,
        [](uv_work_t *work) { /* 线程池执行逻辑 */
            LOGD("------enter uv_work_t Callback");
            CallbackContext *context = (CallbackContext *)work->data;
            if (context && context->vapState == VapState::FAILED) {
                LOGD("------The vap status in the thread is failed");
            } else if (context && (context->vapState == VapState::RENDER || context->vapState == VapState::START)) {
                context->callbackRef();
                LOGD("------The vap status in the thread is RENDER、START");
            } else if (context) {
                LOGD("------The vap status in the thread is RENDER、START");
                context->callbackRef();
            }
            if (work != nullptr) {
                    delete work;
                }
            delete context;
        },
        [](uv_work_t *work, int status) {LOGI("napi-->uv_queue_work end");});
}

typedef void (*Callback1)();
void CangjiePluginRenderPlay(const char *url, CangjieMixData *mixDataCj, size_t length, Callback1 backw, int64_t id);
void CangjiePluginRenderPlay(const char *url, CangjieMixData *mixDataCj, size_t length, Callback1 backw, int64_t id) {
    LOGD("--------Invoke the callback function:");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if(render){
        std::string uri = url;
        if(uri.empty()){
            return;
        }
        VAPInfo info;
        std::map<std::string, MixInputData> mixData;
        ParseMixParam(mixData,mixDataCj,length);
        if(render->player_ == nullptr){
            return;
        }
        render->player_->SetCallback(CallbackType::PLAY_DONE, backw, &Callback);
        info.uri = uri;
        info.window = static_cast<NativeWindow*>(render->m_window);
        info.width = render->m_width;
        info.height = render->m_height;
        info.iptData = mixData;
        render->player_->Init(info);
    }
}

typedef void (*stateCallback)(int64_t,int64_t);
void CangjiePluginRenderOn(const char *str, stateCallback callbackw, int64_t id);
void CangjiePluginRenderOn(const char *str, stateCallback callbackw, int64_t id){
    PluginRender *render = PluginManager::GetPluginRender(id);
    if(render){
        if(render->player_->IsRunning()){
            return;
        }
        std::string newStr = str;
        if(newStr != "stateChange" && newStr != "click"){
            return;
        }
        void (*callback)(void *context) = nullptr;
        callback = &Callback;
        if(callback){
            if(newStr == "click"){
                //  render->player_->SetCallback;
            }else if(newStr == "stateChange"){
                //  render->player_->SetCallback;
            }
        }
    }
}

void CangjiePluginRenderPause(int64_t id);
void CangjiePluginRenderPause(int64_t id) {
    LOGD("enter pause");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if(render){
        render->player_->PauseAndResume();
    }
}

void CangjiePluginRenderStop(int64_t id);
void CangjiePluginRenderStop(int64_t id) {
    LOGD("enter Stop");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (render && render->player_) {
        render->player_->Stop();
    }
    LOGD("end Stop");
}

void CangjiePluginRenderSetLoop(int64_t loops, int64_t id);
void CangjiePluginRenderSetLoop(int64_t loops, int64_t id) {
    LOGD("enter SetLoop");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (render) {
        render->player_->SetLoop(loops);
    }
    LOGD("end SetLoop");
}

void CangjiePluginRenderSetFitType(long long fitTypeInt, int64_t id);
void CangjiePluginRenderSetFitType(long long fitTypeInt, int64_t id) {
    LOGD("enter SetFitType");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
        LOGE("Not get render");
        return;
    }
    VideoFitType fitType = VideoFitType::FIT_XY;
    if (fitTypeInt == 0) {
        fitType = VideoFitType::FIT_XY;
    } else if (fitTypeInt == 1) {
        fitType = VideoFitType::FIT_CENTER;
    } else {
        fitType = VideoFitType::CENTER_CROP;
    }
    render->player_->SetFitType(fitType);
}

SrcInfoArray CangjiePluginRenderGetVideoInfo(const char *url, int64_t id);
SrcInfoArray CangjiePluginRenderGetVideoInfo(const char *url, int64_t id) {
    LOGD("enter GetVideoInfo");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
        LOGE("Not get render");
        return {};
    }
    std::string uri = url;
    AnimConfig animConfig;
    std::vector<SrcInfoo> srcInfo;
    animConfig.ParseJson(uri, false);
    std::map<std::string, Src> src;
    animConfig.GetSrc(src);
    if (!src.empty()) {
        srcInfo = GenSrcInfo(src);
    }
    int64_t i = 0;
    const int64_t size = srcInfo.size();
    int64_t *srcIdInt64 = new int64_t[size];
    int64_t *srcTypeInt64 = new int64_t[size];
    std::vector<CJBytesArray> markers;
    for(const auto &item : srcInfo){
        srcIdInt64[i] = item.srcId;
        srcTypeInt64[i] = item.srcType;
        markers.push_back(item.tag);
        i++;
    }
    if(markers.empty()){
        return SrcInfoArray{0,0,0,{},0,0};
    }

    long long size1 = markers.size();
    CJBytesArray *arr = new CJBytesArray[size1];
    for(size_t i = 0; i<size1;i++){
        arr[i] = markers[i];
    }
    return SrcInfoArray{size, srcIdInt64, srcTypeInt64, CJInt64Array{size, arr}, animConfig.width, animConfig.height };
}

void CangjiePluginRenderSetVideoMode(const int64_t *videoModeInt, int64_t id);
void CangjiePluginRenderSetVideoMode(const int64_t *videoModeInt, int64_t id) {
    LOGD("enter SetVideoMode");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
        LOGE("Not get render");
        return;
    }
    VideoMode videoMode = VIDEO_MODE_SPLIT_HORIZONTAL;
    if (videoModeInt == nullptr) {
        videoMode = VIDEO_MODE_SPLIT_HORIZONTAL;
    } else if (static_cast<int>(*videoModeInt) == ONE) {
        videoMode = VIDEO_MODE_SPLIT_VERTICAL;
    } else if (static_cast<int>(*videoModeInt) == TWO) {
        videoMode = VIDEO_MODE_SPLIT_HORIZONTAL_REVERSE;
    } else {
        videoMode = VIDEO_MODE_SPLIT_VERTICAL_REVERSE;
    }
    render->player_->SetVideoMode(videoMode);
}

CJBytesArray GetsElementAnArray(CJBytesArray *cJBytesArray, int64_t i);
CJBytesArray GetsElementAnArray(CJBytesArray *cJBytesArray, int64_t i){
    return cJBytesArray[i];
}

void CangjiePluginRenderSetSpeed(const float *speedInt, int64_t id);
void CangjiePluginRenderSetSpeed(const float *speedInt, int64_t id) {
    LOGD("enter CangjiePluginRenderSetSpeed");
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
        LOGE("Not get render");
        return;
    }
    float speed = std::clamp(*speedInt, 0.25f, 4.0f);
    render->player_->SetSpeed(speed);
}

EXTERN_C_START
CJ_EXPORT void FFIChangeColor(int64_t surfaceId);
CJ_EXPORT void FFIChangeColor(int64_t surfaceId)
{
    PluginManager::ChangeColor(surfaceId);
}
CJ_EXPORT void FFIDrawPattern(int64_t surfaceId);
CJ_EXPORT void FFIDrawPattern(int64_t surfaceId)
{
    PluginManager::DrawPattern(surfaceId);
}

CJ_EXPORT void FFISetSurfaceId(int64_t surfaceId);
CJ_EXPORT void FFISetSurfaceId(int64_t surfaceId)
{
    PluginManager::SetSurfaceId(surfaceId);
}

CJ_EXPORT void FFIChangeSurface(int64_t surfaceId, float width, float height)
{
    PluginManager::ChangeSurface(surfaceId, width, height);
    PluginRender *render = PluginManager::GetPluginRender(surfaceId);
    render->player_ = std::make_unique<Player>();
    render->m_width = width;
    render->m_height = height;
    if(render->player_->eglCore_){
        render->player_->eglCore_->UpdateSize(width, height);
    }
}

CJ_EXPORT void FFIDestroySurface(int64_t surfaceId);
CJ_EXPORT void FFIDestroySurface(int64_t surfaceId)
{
    PluginManager::DestroySurface(surfaceId);
}

CJ_EXPORT bool FFIGetXComponentHasDraw(int64_t surfaceId);
CJ_EXPORT bool FFIGetXComponentHasDraw(int64_t surfaceId)
{
    return PluginManager::GetXComponentHasDraw(surfaceId);
}

CJ_EXPORT bool FFIXComponentHasChangeColor(int64_t surfaceId);
CJ_EXPORT bool FFIXComponentHasChangeColor(int64_t surfaceId)
{
    return PluginManager::GetXComponentHasChangeColor(surfaceId);
}
EXTERN_C_END
}