/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "plugin_render.h"
#include "util/cj_string_util.h"
#include "plugin_manager.h"
#include <cstdint>
#include <js_native_api.h>
#include <log.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <optional>
#include <string>
#include <uv.h>

std::mutex g_mtx;
std::unordered_map<int64_t, std::shared_ptr<PluginRender>> PluginRender::m_instance;
extern "C" {
PluginRender::PluginRender(int64_t &id)
{
    this->eglCore_ = new EGLCore();
    hasDraw_ = 0;
    hasChangeColor_ = 0;
    this->m_id = id;
}

PluginRender::~PluginRender()
{
    if (player_) {
        LOGI("PluginRender player_");
        player_->StartRelease();
    }
}

void PluginRender::ChangeColor() { eglCore_->ChangeColor(hasChangeColor_); }

void PluginRender::DrawPattern() { eglCore_->Draw(hasDraw_); }

void PluginRender::UpdateNativeWindowSize(int width, int height)
{
    eglCore_->UpdateSize(width, height);
    if (!hasChangeColor_ && !hasDraw_) {
        eglCore_->Background();
    }
}

int32_t PluginRender::HasDraw() { return hasDraw_; }

int32_t PluginRender::HasChangedColor() { return hasChangeColor_; }

static void ParseMixParamTag(std::string tag, std::string &tag1)
{
    if (!tag.empty()) {
        tag1 = tag;
    } else {
        return;
    }
}

static void ParseMixParamTxt(MixInputData &mixInputData, std::string txt)
{
    if (!txt.empty()) {
        mixInputData.txt = txt;
    }
}

static void ParseMixParamImg(MixInputData &mixInputData, std::string imgUri)
{
    if (!imgUri.empty()) {
        mixInputData.imgSrc.buffer = imgUri;
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

static std::optional<OH_Drawing_FontWeight> intToFontWeight(int value)
{
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

static void ParseMixParamFontWeight(MixInputData &mixInputData, int value)
{
    auto fontWeight = intToFontWeight(value);
    if (fontWeight) {
        mixInputData.fontWeight = fontWeight.value();
        mixInputData.isSet |= SET_FONT_WEIGHT;
    }
}

static std::optional<OH_Drawing_TextAlign> intToTextAlign(int value)
{
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

static void ParseMixParamTextAlign(MixInputData &mixInputData, int value)
{
    auto textAlign = intToTextAlign(value);
    if (textAlign) {
        mixInputData.textAlign = textAlign.value();
        mixInputData.isSet |= SET_TEXT_ALIGN;
    }
}

void ParseMixParam(std::map<std::string, MixInputData> &mixData, CangjieMixData *mixDataCj, size_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        // 获取当前结构体元素的引用
        MixInputData mixInputData;
        std::string tag;
        ParseMixParamTag(mixDataCj[i].tag, tag);
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

struct CallbackContext4cj {
    void (*callbackRef)() = nullptr;
    int what = 0;
    int arg1 = 0;
    int arg2 = 0;
    char *obj;
    int ijk_id = 0;
};

CJBytesArray RemoveSurroundingChars(const std::string &str, char startChar, char endChar)
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
    return pag::stringToCJBytesArray((char *)str.c_str());
}

static std::vector<SrcInfoo> GenSrcInfo(std::map<std::string, Src> &src)
{
    std::vector<SrcInfoo> srcInfos;
    size_t i = 0;
    for (auto ele : src) {
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

void freeCJBytesArray(CJBytesArray *cjbytesArray, int64_t i)
{
    if (cjbytesArray[i].buf == nullptr) {
        return;
    }
    uint8_t *buf = cjbytesArray[i].buf;
    if (buf != nullptr) {
        free(buf);
    }
}

void Callback(void *asyncContext)
{
    if (asyncContext == nullptr) {
        return;
    }
    uv_loop_t *loop = nullptr;
    CallbackContext *context = (CallbackContext *)asyncContext;
    uv_work_t *work = new uv_work_t;
    work->data = context;
    if (work == nullptr) {
        return;
    }
    loop = uv_default_loop();
    uv_queue_work(
        loop, work,
        [](uv_work_t *work) { /* 线程池执行逻辑 */
                              CallbackContext *context = (CallbackContext *)work->data;
                              if (context && context->vapState == VapState::FAILED) {
                              } else if (context && (context->vapState == VapState::RENDER ||
                                                     context->vapState == VapState::START)) {
                                  context->callbackRef();
                              } else if (context) {
                                  context->callbackRef();
                              }
        },
        [](uv_work_t *work, int status) {
            if (work != nullptr) {
                delete work;
            }
            delete (CallbackContext *)work->data;
        });
}

void CangjiePluginRenderPlay(const char *url, CangjieMixData *mixDataCj, size_t length, Callback1 backw, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);

    if (render) {
        std::string uri = url;
        if (uri.empty()) {
            return;
        }
        VAPInfo info;
        std::map<std::string, MixInputData> mixData;
        ParseMixParam(mixData, mixDataCj, length);
        if (render->player_ == nullptr) {
            return;
        }
        render->player_->SetCallback(CallbackType::PLAY_DONE, backw, &Callback);
        info.uri = uri;
        info.window = static_cast<NativeWindow *>(render->m_window);
        info.width = render->m_width;
        info.height = render->m_height;
        info.iptData = mixData;
        render->player_->Init(info);
    }
}

void CangjiePluginRenderOn(const char *str, stateCallback callbackw, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (render) {
        if (render->player_->IsRunning()) {
            return;
        }
        std::string newStr = str;
        if (newStr != "stateChange" && newStr != "click") {
            return;
        }
        void (*callback)(void *context) = nullptr;
        callback = &Callback;
        if (callback) {
            if (newStr == "click") {
            } else if (newStr == "stateChange") {
            }
        }
    }
}

void CangjiePluginRenderPause(int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (render) {
        render->player_->PauseAndResume();
    }
}

void CangjiePluginRenderStop(int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (render && render->player_) {
        render->player_->Stop();
    }
}

void CangjiePluginRenderSetLoop(int64_t loops, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (render) {
        render->player_->SetLoop(loops);
    }
}

void CangjiePluginRenderSetFitType(long long fitTypeInt, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
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

SrcInfoArray CangjiePluginRenderGetVideoInfo(const char *url, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
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
    for (const auto &item : srcInfo) {
        srcIdInt64[i] = item.srcId;
        srcTypeInt64[i] = item.srcType;
        markers.push_back(item.tag);
        i++;
    }
    if (markers.empty()) {
        return SrcInfoArray{0, 0, 0, {}, 0, 0};
    }

    long long size1 = markers.size();
    CJBytesArray *arr = new CJBytesArray[size1];
    for (size_t i = 0; i < size1; i++) {
        arr[i] = markers[i];
    }
    return SrcInfoArray{size, srcIdInt64, srcTypeInt64, CJInt64Array{size, arr}, animConfig.width, animConfig.height};
}

void CangjiePluginRenderSetVideoMode(const int64_t *videoModeInt, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
        return;
    }
    VideoMode videoMode = VIDEO_MODE_SPLIT_HORIZONTAL;
    if (videoModeInt == reinterpret_cast<const int64_t *>(0)) {
        videoMode = VIDEO_MODE_SPLIT_HORIZONTAL;
    } else if (videoModeInt == reinterpret_cast<const int64_t *>(1)) {
        videoMode = VIDEO_MODE_SPLIT_VERTICAL;
    } else if (videoModeInt == reinterpret_cast<const int64_t *>(2)) {
        videoMode = VIDEO_MODE_SPLIT_HORIZONTAL_REVERSE;
    } else {
        videoMode = VIDEO_MODE_SPLIT_VERTICAL_REVERSE;
    }
    render->player_->SetVideoMode(videoMode);
}

CJBytesArray GetsElementAnArray(CJBytesArray *cJBytesArray, int64_t i) { return cJBytesArray[i]; }

void CangjiePluginRenderSetSpeed(const float *speedInt, int64_t id)
{
    PluginRender *render = PluginManager::GetPluginRender(id);
    if (!render) {
        return;
    }
    float speed = std::clamp(*speedInt, 0.25f, 4.0f);
    render->player_->SetSpeed(speed);
}

EXTERN_C_START
CJ_EXPORT void FFIChangeColor(int64_t surfaceId) { PluginManager::ChangeColor(surfaceId); }

CJ_EXPORT void FFIDrawPattern(int64_t surfaceId) { PluginManager::DrawPattern(surfaceId); }

CJ_EXPORT void FFISetSurfaceId(int64_t surfaceId) { PluginManager::SetSurfaceId(surfaceId); }

CJ_EXPORT void FFIChangeSurface(int64_t surfaceId, float width, float height)
{
    PluginManager::ChangeSurface(surfaceId, width, height);
    PluginRender *render = PluginManager::GetPluginRender(surfaceId);
    if (nullptr != render) {
        render->player_ = std::make_unique<Player>();
        render->m_width = width;
        render->m_height = height;

        uint64_t actualWidth = 0;
        uint64_t actualHeight = 0;
        OH_NativeXComponent_GetXComponentSize(render->m_component, render->m_window, &actualWidth, &actualHeight);
        if (render->player_ != nullptr && render->player_->eglCore_ != nullptr) {
            render->player_->eglCore_->UpdateSize(actualWidth, actualHeight);
        }
    }
}

CJ_EXPORT void FFIDestroySurface(int64_t surfaceId)
{
    PluginManager::DestroySurface(surfaceId);
}

CJ_EXPORT bool FFIGetXComponentHasDraw(int64_t surfaceId)
{
    return PluginManager::GetXComponentHasDraw(surfaceId);
}

CJ_EXPORT bool FFIXComponentHasChangeColor(int64_t surfaceId)
{
    return PluginManager::GetXComponentHasChangeColor(surfaceId);
}
EXTERN_C_END
}
