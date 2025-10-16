/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#define NATIVE_XCOMPONENT_PLUGIN_MANAGER_H

#include <unordered_map>
#include <native_window/external_window.h>
#include "../render/plugin_render.h"
#include <cstdint>
#include <string>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>

static std::unordered_map<int64_t, PluginRender*> pluginRenderMap_;
static std::unordered_map<int64_t, OHNativeWindow*> windowMap_;

class PluginManager {
public:
    ~PluginManager();

    static PluginManager *GetInstance()
    {
        return &PluginManager::m_pluginManager;
    }
    static void * SetSurfaceId(int64_t surfaceId);
    static PluginRender* GetPluginRender(int64_t &id);
    static void * ChangeColor(int64_t surfaceId);
    static void * DrawPattern(int64_t surfaceId);
    static void * ChangeSurface(int64_t surfaceId, double width,double height);
    static void * DestroySurface(int64_t surfaceId);
    static bool GetXComponentHasDraw(int64_t surfaceId);
    static bool GetXComponentHasChangeColor(int64_t surfaceId);
    
private:
    static PluginManager m_pluginManager;
};
#endif // NATIVE_XCOMPONENT_PLUGIN_MANAGER_H