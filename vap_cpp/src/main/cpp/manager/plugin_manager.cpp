/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include <cstdint>
#include <cstdio>
#include <string>
#include "plugin_manager.h"
#include <ace/xcomponent/native_interface_xcomponent.h>

std::unordered_map<int64_t, PluginRender *> PluginManager::pluginRenderMap_;
std::unordered_map<int64_t, OHNativeWindow *> PluginManager::windowMap_;

static const size_t GET_CONTEXT_PARAM_CNT = 1;

enum ContextType {
    APP_LIFECYCLE,
    PAGE_LIFECYCLE,
};

PluginManager::~PluginManager() { std::unique_lock<std::mutex> lock(mtx_); }
PluginRender *PluginManager::GetPluginRender(int64_t &id)
{
    if (pluginRenderMap_.find(id) != pluginRenderMap_.end()) {
        return pluginRenderMap_[id];
    }
    return nullptr;
}

void *PluginManager::SetSurfaceId(int64_t surfaceId)
{
    OHNativeWindow *nativeWindow;
    PluginRender *pluginRender;
    if (windowMap_.find(surfaceId) == windowMap_.end()) {
        OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &nativeWindow);
        windowMap_[surfaceId] = nativeWindow;
    }
    if (pluginRenderMap_.find(surfaceId) == pluginRenderMap_.end()) {
        pluginRender = new PluginRender(surfaceId);
        pluginRender->m_window = nativeWindow;
        pluginRenderMap_[surfaceId] = pluginRender;
    }
    return nullptr;
}

void *PluginManager::DestroySurface(int64_t surfaceId)
{
    auto pluginRender = GetPluginRender(surfaceId);
    if (nullptr != pluginRender) {
        pluginRender->player_->NoticeDestroyed();
    }
    auto pluginRenderMapIter = pluginRenderMap_.find(surfaceId);
    if (pluginRenderMapIter != pluginRenderMap_.end()) {
        delete pluginRenderMapIter->second;
        pluginRenderMap_.erase(pluginRenderMapIter);
    }
    auto windowMapIter = windowMap_.find(surfaceId);
    if (windowMapIter != windowMap_.end()) {
        pluginRender->surfaceDestroyed_ = true;
        OH_NativeWindow_DestroyNativeWindow(windowMapIter->second);
        windowMap_.erase(windowMapIter);
    }
    return nullptr;
}

void *PluginManager::ChangeSurface(int64_t surfaceId, double width, double height)
{
    auto pluginRender = GetPluginRender(surfaceId);
    if (pluginRender == nullptr) {
        return nullptr;
    }
    pluginRender->UpdateNativeWindowSize(width, height);
    return nullptr;
}

void *PluginManager::ChangeColor(int64_t surfaceId)
{
    auto pluginRender = GetPluginRender(surfaceId);
    if (pluginRender == nullptr) {
        return nullptr;
    }
    pluginRender->ChangeColor();
    return nullptr;
}

void *PluginManager::DrawPattern(int64_t surfaceId)
{
    auto pluginRender = GetPluginRender(surfaceId);
    if (pluginRender == nullptr) {
        return nullptr;
    }
    pluginRender->DrawPattern();
    return nullptr;
}

bool PluginManager::GetXComponentHasDraw(int64_t surfaceId)
{
    auto pluginRender = GetPluginRender(surfaceId);
    if (pluginRender == nullptr) {
        return false;
    }
    return pluginRender->HasDraw();
}

bool PluginManager::GetXComponentHasChangeColor(int64_t surfaceId)
{
    auto pluginRender = GetPluginRender(surfaceId);
    if (pluginRender == nullptr) {
        return false;
    }
    return pluginRender->HasChangedColor();
}