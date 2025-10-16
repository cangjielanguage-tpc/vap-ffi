/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_CANGJIE_PLUGIN_RENDER_H
#define VAP_CANGJIE_PLUGIN_RENDER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "player.h"

class PluginRenderCangjieFFI {
public:
    explicit PluginRenderCangjieFFI(std::string &id);
    ~PluginRenderCangjieFFI()
    {
        if (player_) {
            player_->StartRelease();  // 确保播放器资源释放
        }
        m_instance.clear();  // 清理实例（需确认设计合理性）
    }
    static std::shared_ptr<PluginRenderCangjieFFI> GetInstance(std::string &id);
    static void Release(std::string &id);
    

public:
    static std::unordered_map<std::string, std::shared_ptr<PluginRenderCangjieFFI>> m_instance; // 标个点
    static OH_NativeXComponent_Callback m_callback;
    
    std::string m_id;
    void *m_window = nullptr;
    OH_NativeXComponent *m_component = nullptr; // OpenHarmony 原生组件
    uint64_t m_width;
    uint64_t m_height;
    std::unique_ptr<Player> player_ = nullptr;  // 播放器对象
    bool surfaceDestroyed_ = false;
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
//void CangjiePluginRenderInit(const char* str);  
//播放指定 URL 的多媒体内容，支持混合数据（如叠加文字/图片），并通过回调函数通知结果。
//void CangjiePluginRenderPlay(const char* url, struct CangjieMixData data[], void (*callbackFn)()); 
//
//void CangjiePluginRenderOn(const char* url, struct CangjieMixData data[], void (*callbackFn)());
//
//void CangjiePluginRenderPause();
//
//void CangjiePluginRenderStop(const char* str);
//
//void CangjiePluginRenderSetLoop(const int64_t* loops);
//
//void CangjiePluginRenderSetFitType(const int64_t* fitTypeInt);
//
//void CangjiePluginRenderGetVideoInfo(const char* url);
//
//void CangjiePluginRenderSetVideoMode(const int64_t* videoModeInt);
//
//void CangjiePluginRenderSetSpeed(const int64_t* speedInt);

//void CangjiePluginRenderOff(const char* url, struct CangjieMixData data[], void (*callbackFn)());
}
#endif // VAP_CANGJIE_PLUGIN_RENDER_H
