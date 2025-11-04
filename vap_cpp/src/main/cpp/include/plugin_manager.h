/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#define NATIVE_XCOMPONENT_PLUGIN_MANAGER_H

#include <string>
#include <unordered_map>

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>
#include "plugin_render.h"
//static std::unordered_map<int64_t, PluginRender*> pluginRenderMap_;
//static std::unordered_map<int64_t, OHNativeWindow*> windowMap_;

class PluginManager {
public:
    ~PluginManager();
    static PluginRender* GetPluginRender(int64_t &id);
    static void * ChangeColor(int64_t surfaceId);
    static void * DrawPattern(int64_t surfaceId);
    static void * SetSurfaceId(int64_t surfaceId);
    static void * ChangeSurface(int64_t surfaceId, double width,double height);
    static void * DestroySurface(int64_t surfaceId);
    static bool GetXComponentHasDraw(int64_t surfaceId);
    static bool GetXComponentHasChangeColor(int64_t surfaceId);
    
private:
    static std::unordered_map<int64_t, PluginRender*> pluginRenderMap_;
    static std::unordered_map<int64_t, OHNativeWindow*> windowMap_;
    std::mutex mtx_;
//    std::unordered_map<std::string, std::shared_ptr<PluginRender>> m_pluginRenderMap;
//    static PluginManager m_pluginManager;
};
#endif // NATIVE_XCOMPONENT_PLUGIN_MANAGER_H