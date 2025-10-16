/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_DEMUXER_H
#define VAP_DEMUXER_H

#include <bits/alltypes.h>
#include "multimedia/player_framework/native_avdemuxer.h"
#include "data_info.h"

static constexpr int32_t ONE_K = 1024;

class Demuxer {
public:
    ~Demuxer();
    int32_t CreateDemuxer(VAPInfo &sampleInfo);
    int32_t GetTrackInfo(std::shared_ptr<OH_AVFormat> sourceFormat, VAPInfo &info);
    int32_t ReadSample(OH_AVBuffer *buffer, OH_AVCodecBufferAttr &attr);
    int32_t ReadAudioSample(OH_AVBuffer *buffer, OH_AVCodecBufferAttr &attr);
    int32_t Release();

    bool hasAudio() { return audioTrackId_ != -1; }
    bool hasVideo() { return videoTrackId_ != -1; }

private:
    OH_AVSource *source_ = nullptr;
    OH_AVDemuxer *demuxer_ = nullptr;
    int32_t videoTrackId_ = -1;
    int32_t audioTrackId_ = -1;
};

#endif // VAP_DEMUXER_H