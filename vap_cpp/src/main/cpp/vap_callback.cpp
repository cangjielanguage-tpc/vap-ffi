/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "vap_callback.h"
#include "log.h"

namespace {
constexpr int LIMIT_LOGD_FREQUENCY = 50;
}

// 自定义写入数据函数
int32_t VAPCallback::OnRenderWriteData(OH_AudioRenderer *renderer, void *userData, void *buffer, int32_t length)
{
    if (userData == nullptr || buffer == nullptr) {
        LOGD("OnRenderWriteData con not get userData");
        return 1;
    }
    (void)renderer;
    ADecSignal *codecUserData = static_cast<ADecSignal *>(userData);
    uint8_t *dest = (uint8_t *)buffer;
    size_t index = 0;
    std::unique_lock<std::mutex> lock(codecUserData->audioOutputMutex);
    
    // 优化：计算可拷贝的最大长度，批量拷贝（需确保队列底层是连续内存，如std::deque）
    size_t copySize = std::min((size_t)length, codecUserData->renderQueue.size());
    if (copySize > 0) {
        // 若使用std::deque，可通过地址连续的一段数据直接拷贝
        memcpy(dest, &codecUserData->renderQueue.front(), copySize);
        index = copySize;
        // 批量弹出已拷贝数据
        for (size_t i = 0; i < copySize; ++i) {
            codecUserData->renderQueue.pop();
        }
    }
    
    LOGD("render BufferLength:%{public}d, renderQueue.size: %{public}lu, renderReadSize: %{public}lu",
         length, codecUserData->renderQueue.size(), index);
    if (codecUserData->renderQueue.size() < (size_t)length) {
        codecUserData->renderCond.notify_all();
    }
    return 0;
}

// 自定义音频流事件函数
int32_t VAPCallback::OnRenderStreamEvent(OH_AudioRenderer *renderer, void *userData, OH_AudioStream_Event event)
{
    (void)renderer;
    (void)userData;
    (void)event;
    // 根据event表示的音频流事件信息，更新播放器状态和界面
    return 0;
}
// 自定义音频中断事件函数
int32_t VAPCallback::OnRenderInterruptEvent(OH_AudioRenderer *renderer,
    void *userData, OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
{
    (void)renderer;
    (void)userData;
    (void)type;
    (void)hint;
    if (userData == nullptr) {
        LOGD("OnRenderWriteData con not get userData");
        return 1;
    }
    ADecSignal *codecUserData = static_cast<ADecSignal *>(userData);
    codecUserData->isInterrupt = true;
    LOGD("audio RenderInterruptEvent type: %{public}d hint: %{public}d", type, hint);
    return 0;
}
// 自定义异常回调函数
int32_t VAPCallback::OnRenderError(OH_AudioRenderer *renderer, void *userData, OH_AudioStream_Result error)
{
    (void)renderer;
    (void)userData;
    (void)error;
    LOGE("OnRenderError");
    // 根据error表示的音频异常信息，做出相应的处理
    return 0;
}

void VAPCallback::OnCodecError(OH_AVCodec *codec, int32_t errorCode, void *userData)
{
    (void)codec;
    (void)errorCode;
    (void)userData;
    LOGD("On codec error, error code: %{public}d", errorCode);
}

void VAPCallback::OnCodecFormatChange(OH_AVCodec *codec, OH_AVFormat *format, void *userData)
{
    LOGD("On codec format change");
}

void VAPCallback::OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData)
{
    if (userData == nullptr) {
        return;
    }
    (void)codec;
    VDecSignal *codecUserData = static_cast<VDecSignal *>(userData);
    std::unique_lock<std::mutex> lock(codecUserData->inputMutex);
    codecUserData->inputBufferInfoQueue.emplace(index, buffer);
    codecUserData->inputCond.notify_all();
}

void VAPCallback::OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData)
{
    if (userData == nullptr) {
        return;
    }
    (void)codec;
    VDecSignal *codecUserData = static_cast<VDecSignal *>(userData);
    std::unique_lock<std::mutex> lock(codecUserData->outputMutex);
    codecUserData->outputBufferInfoQueue.emplace(index, buffer, codec);
    codecUserData->outputCond.notify_all();
}
