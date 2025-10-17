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
    ~PluginRender()
    {
        if(eglCore_ != nullptr){
            eglCore_->Release();
            delete eglCore_;
            eglCore_ = nullptr;
        }
        if (player_) {
            player_->StartRelease();
        }
        m_instance.clear();
    }
    static std::shared_ptr<PluginRender> GetInstance(int64_t &id);
    void ChangeColor();
    void DrawPattern();
    int32_t HasDraw();
    int32_t HasChangedColor();
    void UpdateNativeWindowSize(int width, int height);

    static std::unordered_map<int64_t, std::shared_ptr<PluginRender>> m_instance;// 标个点
    static OH_NativeXComponent_Callback m_callback;
    
    std::string m_id;
    void *m_window = nullptr;
    OH_NativeXComponent *m_component = nullptr;
    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<Player> player_ = nullptr;
    
    bool surfaceDestroyed_ = false;
    
private:
    EGLCore* eglCore_;
    int32_t hasDraw_;
    int32_t hasChangeColor_;
};
#endif // NATIVE_XCOMPONENT_PLUGIN_RENDER_H
