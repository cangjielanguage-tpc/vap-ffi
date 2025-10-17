/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#include "plugin_manager.h"
#include <cstdint>
#include <string>
#include <cstdio>

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <hilog/log.h>

PluginManager PluginManager::m_pluginManager;

//static const size_t;
static const size_t GET_CONTEXT_PARAM_CNT = 1;
typedef void (*stateCallback)(int64_t);

enum ContextType {
    APP_LIFECYCLE,
    PAGE_LIFECYCLE,
};

PluginManager::~PluginManager()
{
    for (auto iter = pluginRenderMap_.begin(); iter != pluginRenderMap_.end(); ++iter) {
        if (nullptr != iter->second) {
            iter->second = nullptr;
        }
    }
    pluginRenderMap_.clear();
    for (auto iter = windowMap_.begin(); iter != windowMap_.end(); ++iter) {
        if (nullptr != iter->second) {
            iter->second = nullptr;
        }
    }
    windowMap_.clear();
}

PluginRender *PluginManager::GetPluginRender(int64_t &id){
    
    if(pluginRenderMap_.find(id) != pluginRenderMap_.end()){
        return pluginRenderMap_[id];
    }
    return nullptr;
}

void *PluginManager::SetSurfaceId(int64_t surfaceId){
    OHNativeWindow *nativeWindow;
    PluginRender *pluginRender;
    if(windowMap_.find(surfaceId) == windowMap_.end()){
        OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &nativeWindow);
        windowMap_[surfaceId] = nativeWindow;
    }
    if(pluginRenderMap_.find(surfaceId) == pluginRenderMap_.end()){
        pluginRender = new PluginRender(surfaceId);
        pluginRender->m_window = nativeWindow;
        pluginRenderMap_[surfaceId] = pluginRender;
    }
    return nullptr;
}

void *PluginManager::DestroySurface(int64_t surfaceId){
    auto pluginRenderMapIter = pluginRenderMap_.find(surfaceId);
    if(pluginRenderMapIter != pluginRenderMap_.end()){
        delete pluginRenderMapIter->second;
        pluginRenderMap_.erase(pluginRenderMapIter);
    }
    auto windowMapIter = windowMap_.find(surfaceId);
    if(windowMapIter != windowMap_.end()){
        OH_NativeWindow_DestroyNativeWindow(windowMapIter->second);
        windowMap_.erase(windowMapIter);
    }
    return nullptr;
}

void *PluginManager::ChangeSurface(int64_t surfaceId, double width, double height){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        return nullptr;
    }
    pluginRender->UpdateNativeWindowSize(width, height);
    return nullptr;
}

void *PluginManager::ChangeColor(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        return nullptr;
    }
    pluginRender->ChangeColor();
    return nullptr;
}

void *PluginManager::DrawPattern(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        return nullptr;
    }
    pluginRender->DrawPattern();
    return nullptr;
}

bool PluginManager::GetXComponentHasDraw(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        return false;
    }
    return pluginRender->HasDraw();
}

bool PluginManager::GetXComponentHasChangeColor(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        return false;
    }
    return pluginRender->HasChangedColor();
}