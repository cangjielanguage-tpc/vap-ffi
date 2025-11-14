/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NATIVE_XCOMPONENT_PLUGIN_RENDER_H
#define NATIVE_XCOMPONENT_PLUGIN_RENDER_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <native_window/external_window.h>
#include <string>
#include <unordered_map>
#include "egl_core.h"
#include "player.h"

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
    void InitNativeWindow(OHNativeWindow* window);
    void UpdateNativeWindowSize(int width, int height);
    
    static void Release(int64_t &id);

    static std::unordered_map<int64_t, std::shared_ptr<PluginRender>> m_instance;
    
    std::string m_id;
    std::function<void(const int64_t&)> deleteRenderCallback_ {nullptr};
    void *m_window = nullptr;
    OH_NativeXComponent *m_component = nullptr;
    uint64_t m_width;
    uint64_t m_height;
    std::unique_ptr<Player> player_ = nullptr;
    
    static std::unordered_map<int64_t, PluginRender*> pluginRenderMap_;
    static std::unordered_map<int64_t, OHNativeWindow*> windowMap_;
    
    static void * ChangeColor(int64_t surfaceId);
    static void * DrawPattern(int64_t surfaceId);
    static void * SetSurfaceId(int64_t surfaceId);
    static void * ChangeSurface(int64_t surfaceId, double width,double height);
    static void * DestroySurface(int64_t surfaceId);
    static bool GetXComponentHasDraw(int64_t surfaceId);
    static bool GetXComponentHasChangeColor(int64_t surfaceId);
    
    bool surfaceDestroyed_ = false;
    
private:
    EGLCore* eglCore_;
    int32_t hasDraw_;
    int32_t hasChangeColor_;
};

extern "C" { 
typedef struct  {
    const char* tag;
    const char* txt;
    const char* imgUri;
    int64_t colorA;
    int64_t colorR;
    int64_t colorG;
    int64_t colorB;
    int32_t VAPFontWeight;
    int32_t VAPTextAlign;
} CangjieMixData ;
// 始化渲染器
};
#endif // NATIVE_XCOMPONENT_PLUGIN_RENDER_H
