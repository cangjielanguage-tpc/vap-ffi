/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include <cstdint>
#include <string>
#include <cstdio>

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <hilog/log.h>
#include "plugin_manager.h"
#include "common/common.h"

//PluginManager PluginManager::m_pluginManager;
std::unordered_map<int64_t, PluginRender*> PluginManager::pluginRenderMap_;
std::unordered_map<int64_t, OHNativeWindow*> PluginManager::windowMap_;

//static const size_t;
static const size_t GET_CONTEXT_PARAM_CNT = 1;
typedef void (*stateCallback)(int64_t);

enum ContextType {
    APP_LIFECYCLE,
    PAGE_LIFECYCLE,
};

PluginManager::~PluginManager()
{
    LOGD("~PluginManager");
    std::unique_lock<std::mutex> lock(mtx_);
//    m_pluginRenderMap.clear();
}

PluginRender *PluginManager::GetPluginRender(int64_t &id){
    LOGE("GetPluginRender,id: %{public}ld",id);
    LOGE("GetPluginRender,pluginRenderMap_.size: %{public}ld",pluginRenderMap_.size());
    if(pluginRenderMap_.find(id) != pluginRenderMap_.end()){
        LOGE("GetPluginRender,pluginRenderMap_.size: %{public}ld",pluginRenderMap_.size());
        return pluginRenderMap_[id];
    }
    return nullptr;
}

void *PluginManager::SetSurfaceId(int64_t surfaceId){
    OHNativeWindow *nativeWindow;
    PluginRender *pluginRender;
    LOGE("SetSurfaceId,surfaceId: %{public}ld",surfaceId);
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
    auto pluginRender = GetPluginRender(surfaceId);
    if (nullptr != pluginRender) {
        pluginRender->player_->NoticeDestroyed();
    }
    auto pluginRenderMapIter = pluginRenderMap_.find(surfaceId);
    if(pluginRenderMapIter != pluginRenderMap_.end()){
        delete pluginRenderMapIter->second;
        LOGE("player------20,pluginRenderMap_ size: %{public}zu",pluginRenderMap_.size());
        pluginRenderMap_.erase(pluginRenderMapIter);
        LOGE("player------21,pluginRenderMap_ size: %{public}zu",pluginRenderMap_.size());
    }
    auto windowMapIter = windowMap_.find(surfaceId);
    if(windowMapIter != windowMap_.end()){
        pluginRender->surfaceDestroyed_ = true;
        OH_NativeWindow_DestroyNativeWindow(windowMapIter->second);
        windowMap_.erase(windowMapIter);
    }
    
//    PluginRender::Release(id);
    return nullptr;
}

void *PluginManager::ChangeSurface(int64_t surfaceId, double width, double height){
    LOGE("ChangeSurface,surfaceId: %{public}ld",surfaceId);
     auto pluginRender = GetPluginRender(surfaceId);
    LOGE("ChangeSurface,pluginRender == nullptr1: %{public}ld",pluginRender == nullptr);
    if(pluginRender == nullptr){
        LOGE("ChangeSurface,pluginRender == nullptr2: %{public}ld",pluginRender == nullptr);
        return nullptr;
    }
    LOGE("ChangeSurface,width: %{public}ld",width);
    pluginRender->UpdateNativeWindowSize(width, height);
    return nullptr;
}

void *PluginManager::ChangeColor(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "ChangeColor: Get pluginRender failed");
        return nullptr;
    }
    pluginRender->ChangeColor();
    return nullptr;
}

void *PluginManager::DrawPattern(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "DrawPattern: Get pluginRender failed");
        return nullptr;
    }
    pluginRender->DrawPattern();
    return nullptr;
}

bool PluginManager::GetXComponentHasDraw(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager",
                     "GetXComponentStatus: Get pluginRender failed");
        return false;
    }
    return pluginRender->HasDraw();
}

bool PluginManager::GetXComponentHasChangeColor(int64_t surfaceId){
    auto pluginRender = GetPluginRender(surfaceId);
    if(pluginRender == nullptr){
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager",
                     "GetXComponentStatus: Get pluginRender failed");
        return false;
    }
    return pluginRender->HasChangedColor();
}