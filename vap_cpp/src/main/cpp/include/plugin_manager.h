/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#define NATIVE_XCOMPONENT_PLUGIN_MANAGER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>
#include "plugin_render.h"

class PluginManager {
public:
    ~PluginManager();
    // 返回 shared_ptr 副本：调用方在整个使用期内持有强引用，
    // 即使另一线程并发 DestroySurface（erase+析构），对象也不会在使用中被释放，消除悬垂指针 UAF
    static std::shared_ptr<PluginRender> GetPluginRender(int64_t &id);
    // 静默查询（不打日志）：供每帧高频轮询等热路径使用，避免 GetPluginRender 的日志洪泛
    static std::shared_ptr<PluginRender> FindPluginRender(int64_t id);
    static void * ChangeColor(int64_t surfaceId);
    static void * DrawPattern(int64_t surfaceId);
    static void * SetSurfaceId(int64_t surfaceId);
    static void * ChangeSurface(int64_t surfaceId, double width,double height);
    static void * DestroySurface(int64_t surfaceId);
    static bool GetXComponentHasDraw(int64_t surfaceId);
    static bool GetXComponentHasChangeColor(int64_t surfaceId);
    
private:
    static std::unordered_map<int64_t, std::shared_ptr<PluginRender>> pluginRenderMap_;
    static std::unordered_map<int64_t, OHNativeWindow*> windowMap_;
    // 保护 pluginRenderMap_ / windowMap_ 的跨线程并发访问（主线程 Set/Destroy + 工作线程 Get/Find）
    static std::mutex mtx_;
};
#endif // NATIVE_XCOMPONENT_PLUGIN_MANAGER_H