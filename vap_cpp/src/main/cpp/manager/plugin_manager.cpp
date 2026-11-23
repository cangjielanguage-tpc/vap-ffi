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

std::unordered_map<int64_t, std::shared_ptr<PluginRender>> PluginManager::pluginRenderMap_;
std::unordered_map<int64_t, OHNativeWindow*> PluginManager::windowMap_;
std::mutex PluginManager::mtx_;

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
}

std::shared_ptr<PluginRender> PluginManager::GetPluginRender(int64_t &id){
    std::lock_guard<std::mutex> lock(mtx_);
    LOGE("GetPluginRender,id: %{public}ld",id);
    LOGE("GetPluginRender,pluginRenderMap_.size: %{public}ld",pluginRenderMap_.size());
    auto it = pluginRenderMap_.find(id);
    if(it != pluginRenderMap_.end()){
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<PluginRender> PluginManager::FindPluginRender(int64_t id){
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = pluginRenderMap_.find(id);
    if(it != pluginRenderMap_.end()){
        return it->second;
    }
    return nullptr;
}

void *PluginManager::SetSurfaceId(int64_t surfaceId){
    std::lock_guard<std::mutex> lock(mtx_);
    LOGE("SetSurfaceId,surfaceId: %{public}ld",surfaceId);
    if(windowMap_.find(surfaceId) == windowMap_.end()){
        OHNativeWindow *nativeWindow = nullptr;
        OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &nativeWindow);
        windowMap_[surfaceId] = nativeWindow;
    }
    if(pluginRenderMap_.find(surfaceId) == pluginRenderMap_.end()){
        auto pluginRender = std::make_shared<PluginRender>(surfaceId);
        pluginRender->m_window = windowMap_[surfaceId];
        pluginRenderMap_[surfaceId] = pluginRender;
    }
    return nullptr;
}


void *PluginManager::DestroySurface(int64_t surfaceId){
    LOGI("DestroySurface,surfaceId: %{public}ld",surfaceId);
    std::shared_ptr<PluginRender> pluginRender;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto renderIter = pluginRenderMap_.find(surfaceId);
        if (renderIter != pluginRenderMap_.end()) {
            pluginRender = renderIter->second;
            // 锁内摘除映射：此后的 Get/Find 拿不到该条目，不再产生新的引用
            pluginRenderMap_.erase(renderIter);
        }
    }
    if (nullptr != pluginRender) {
        LOGI("DestroySurface,NoticeDestroyed");
        if (nullptr != pluginRender->player_) {
            // 置停止标志并停线程：渲染线程退出链路中 Release/ReleaseAudio 会 join 解码线程
            pluginRender->player_->NoticeDestroyed();
        }
        pluginRender->surfaceDestroyed_ = true;
        // 引用计数释放（检视建议的延迟释放模型）：
        // 常规路径此刻无在途调用，reset 触发 ~PluginRender→~Player→同步 join 渲染线程，
        // 保证下方销毁 window 之前 player 各线程已全部退出；
        // 若工作线程（doneFn2→SetPlayDone）/轮询正持有 shared_ptr 副本，对象存活至其调用结束，
        // 两种时序都不会再出现"持裸指针期间对象被 delete"的 UAF
        pluginRender.reset();
    }
    OHNativeWindow *window = nullptr;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto windowMapIter = windowMap_.find(surfaceId);
        if(windowMapIter != windowMap_.end()){
            window = windowMapIter->second;
            windowMap_.erase(windowMapIter);
        }
    }
    if (nullptr != window) {
        LOGE("windowMapIter != windowMap_.end()");
        OH_NativeWindow_DestroyNativeWindow(window);
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