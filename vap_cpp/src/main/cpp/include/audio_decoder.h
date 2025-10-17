/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_AUDIO_DECODER_H
#define VAP_AUDIO_DECODER_H

#include "multimedia/player_framework/native_avcodec_videodecoder.h"
#include "multimedia/player_framework/native_avbuffer_info.h"
#include "data_info.h"

class AudioDecoder {
public:
    AudioDecoder() = default;
    ~AudioDecoder();

    int32_t CreateAudioDecoder(const std::string &codecMime);
    int32_t ConfigureAudioDecoder(const VAPInfo &sampleInfo);
    int32_t Config(const VAPInfo &sampleInfo, ADecSignal *signal);
    int32_t StartAudioDecoder();
    int32_t PushInputData(AudioCodecBufferInfo &info);
    int32_t FreeOutputData(uint32_t bufferIndex);
    int32_t Release();

private:
    int32_t SetCallback(ADecSignal *signal);

    bool isAVBufferMode_ = false;
    OH_AVCodec *decoder_ = nullptr;
};
#endif // VAP_AUDIO_DECODER_H