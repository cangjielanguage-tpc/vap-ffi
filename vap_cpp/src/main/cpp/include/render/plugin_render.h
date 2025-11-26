/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NATIVE_XCOMPONENT_PLUGIN_RENDER_H
#define NATIVE_XCOMPONENT_PLUGIN_RENDER_H

#include "egl_core.h"
#include "player.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <native_window/external_window.h>
#include <string>
#include <unordered_map>

#ifndef CJ_EXPORT
#ifdef WINDOWS_PLATFORM
#define CJ_EXPORT __declspec(dllexport)
#else
#define CJ_EXPORT __attribute__((visibility("default")))
#endif
#endif

const std::string PROP_TAG = "tag";
const std::string PROP_TXT = "txt";
const std::string PROP_IMAGE = "imgUri";
const std::string PROP_COLOR = "color";
const std::string PROP_FONT_WEIGHT = "fontWeight";
const std::string PROP_TEXT_ALIGN = "textAlign";

class PluginRender {
public:
    explicit PluginRender(int64_t &id);
    ~PluginRender();
    void ChangeColor();
    void DrawPattern();
    int32_t HasDraw();
    int32_t HasChangedColor();
    void UpdateNativeWindowSize(int width, int height);
    static std::unordered_map<int64_t, std::shared_ptr<PluginRender>> m_instance;
    std::string m_id;
    std::function<void(const int64_t &)> deleteRenderCallback_{nullptr};
    void *m_window = nullptr;
    OH_NativeXComponent *m_component = nullptr;
    uint64_t m_width;
    uint64_t m_height;
    std::unique_ptr<Player> player_ = nullptr;
    static std::unordered_map<int64_t, PluginRender *> pluginRenderMap_;
    static std::unordered_map<int64_t, OHNativeWindow *> windowMap_;
    bool surfaceDestroyed_ = false;

private:
    EGLCore *eglCore_;
    int32_t hasDraw_;
    int32_t hasChangeColor_;
};

extern "C" {
typedef struct {
    const char *tag;
    const char *txt;
    const char *imgUri;
    int64_t colorA;
    int64_t colorR;
    int64_t colorG;
    int64_t colorB;
    int32_t VAPFontWeight;
    int32_t VAPTextAlign;
} CangjieMixData;

typedef void (*Callback1)();
typedef void (*stateCallback)(int64_t, int64_t);
void freeCJBytesArray(CJBytesArray *cjbytesArray, int64_t i);
void CangjiePluginRenderPlay(const char *url, CangjieMixData *mixDataCj, size_t length, Callback1 backw, int64_t id);
void CangjiePluginRenderOn(const char *str, stateCallback callbackw, int64_t id);
void CangjiePluginRenderPause(int64_t id);
void CangjiePluginRenderStop(int64_t id);
void CangjiePluginRenderSetLoop(int64_t loops, int64_t id);
void CangjiePluginRenderSetFitType(long long fitTypeInt, int64_t id);
SrcInfoArray CangjiePluginRenderGetVideoInfo(const char *url, int64_t id);
void CangjiePluginRenderSetVideoMode(const int64_t *videoModeInt, int64_t id);
CJBytesArray GetsElementAnArray(CJBytesArray *cJBytesArray, int64_t i);
void CangjiePluginRenderSetSpeed(const float *speedInt, int64_t id);
CJ_EXPORT void FFIChangeColor(int64_t surfaceId);
CJ_EXPORT void FFIDrawPattern(int64_t surfaceId);
CJ_EXPORT void FFISetSurfaceId(int64_t surfaceId);
CJ_EXPORT void FFIChangeSurface(int64_t surfaceId, float width, float height);
CJ_EXPORT void FFIDestroySurface(int64_t surfaceId);
CJ_EXPORT bool FFIGetXComponentHasDraw(int64_t surfaceId);
CJ_EXPORT bool FFIXComponentHasChangeColor(int64_t surfaceId);
};
#endif // NATIVE_XCOMPONENT_PLUGIN_RENDER_H
