/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_DECODER_H
#define VAP_DECODER_H

#include "data_info.h"

class VideoDecoder {
public:
    VideoDecoder() = default;
    ~VideoDecoder();

    int32_t CreateVideoDecoder(const std::string &codecMime);
    int32_t ConfigureVideoDecoder(const VAPInfo &info);
    int32_t Config(const VAPInfo &info, VDecSignal *signal);
    int32_t StartVideoDecoder();
    int32_t PushInputData(CodecBufferInfo &info);
    int32_t FreeOutputData(uint32_t bufferIndex, bool render);
    int32_t Release();

private:
    int32_t SetCallback(VDecSignal *signal);

    bool isAVBufferMode_ = false;
    OH_AVCodec *decoder_ = nullptr;
};
#endif // VAP_DECODER_H