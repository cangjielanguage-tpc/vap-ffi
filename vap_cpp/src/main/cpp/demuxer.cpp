/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include <log.h>
#include "demuxer.h"
#define OHOSCHARPTR char*

Demuxer::~Demuxer()
{
    Release();
}

int32_t Demuxer::CreateDemuxer(VAPInfo &info)
{
    source_ = OH_AVSource_CreateWithFD(info.inputFd, info.inputFileOffset, info.inputFileSize);
    if (source_ == nullptr) {
        return AV_ERR_UNKNOWN;
    }

    demuxer_ = OH_AVDemuxer_CreateWithSource(source_);
    if (demuxer_ == nullptr) {
        return AV_ERR_UNKNOWN;
    }

    auto sourceFormat = std::shared_ptr<OH_AVFormat>(OH_AVSource_GetSourceFormat(source_), OH_AVFormat_Destroy);
    if (sourceFormat == nullptr) {
        return AV_ERR_UNKNOWN;
    }

    int32_t ret = GetTrackInfo(sourceFormat, info);
    if (ret != AV_ERR_OK) {
        return AV_ERR_UNKNOWN;
    }
    return AV_ERR_OK;
}

int32_t Demuxer::GetTrackInfo(std::shared_ptr<OH_AVFormat> sourceFormat, VAPInfo &info)
{
    int32_t trackCount = 0;
    OH_AVFormat_GetIntValue(sourceFormat.get(), OH_MD_KEY_TRACK_COUNT, &trackCount);
    for (int32_t index = 0; index < trackCount; index++) {
        int trackType = -1;
        auto trackFormat = std::shared_ptr<OH_AVFormat>(OH_AVSource_GetTrackFormat(source_, index),
                                                        OH_AVFormat_Destroy);
        OH_AVFormat_GetIntValue(trackFormat.get(), OH_MD_KEY_TRACK_TYPE, &trackType);
        if (trackType == MEDIA_TYPE_VID) {
            OH_AVDemuxer_SelectTrackByID(demuxer_, index);
            OH_AVFormat_GetIntValue(trackFormat.get(), OH_MD_KEY_WIDTH, &info.videoWidth);
            OH_AVFormat_GetIntValue(trackFormat.get(), OH_MD_KEY_HEIGHT, &info.videoHeight);
            OH_AVFormat_GetDoubleValue(trackFormat.get(), OH_MD_KEY_FRAME_RATE, &info.frameRate);
            OH_AVFormat_GetLongValue(trackFormat.get(), OH_MD_KEY_BITRATE, &info.bitrate);
            OH_AVFormat_GetIntValue(trackFormat.get(), "video_is_hdr_vivid", &info.isHDRVivid);
            OH_AVFormat_GetIntValue(trackFormat.get(), OH_MD_KEY_ROTATION, &info.rotation);
            OHOSCHARPTR codecMime;
            OH_AVFormat_GetStringValue(trackFormat.get(), OH_MD_KEY_CODEC_MIME, const_cast<char const **>(&codecMime));
            info.codecMime = codecMime;
            OH_AVFormat_GetIntValue(trackFormat.get(), OH_MD_KEY_PROFILE, &info.hevcProfile);
            videoTrackId_ = index;

        } else if (trackType == MEDIA_TYPE_AUD) {
            OH_AVDemuxer_SelectTrackByID(demuxer_, index);
            OH_AVFormat_GetLongValue(trackFormat.get(), OH_MD_KEY_BITRATE, &info.audioBitrate);
            OH_AVFormat_GetIntValue(trackFormat.get(),
                OH_MD_KEY_AUD_SAMPLE_RATE, reinterpret_cast<int32_t *>(&info.sampleRate));
            OH_AVFormat_GetIntValue(trackFormat.get(),
                OH_MD_KEY_AUD_CHANNEL_COUNT, reinterpret_cast<int32_t *>(&info.channelCount));

            OHOSCHARPTR audioCodecMime;
            OH_AVFormat_GetStringValue(trackFormat.get(),
                OH_MD_KEY_CODEC_MIME, const_cast<char const **>(&audioCodecMime));
            info.audioCodecMime = audioCodecMime;
            audioTrackId_ = index;

            LOGI("Audio Demuxer config: %{public}d %{public}d, %{public}ld"
                 "kbps",
                 info.channelCount, info.sampleRate, info.audioBitrate / ONE_K);
        }
    }
    return AV_ERR_OK;
}

int32_t Demuxer::ReadSample(OH_AVBuffer *buffer, OH_AVCodecBufferAttr &attr)
{
    int32_t ret = OH_AVDemuxer_ReadSampleBuffer(demuxer_, videoTrackId_, buffer);
    if (ret != AV_ERR_OK) {
        return AV_ERR_UNKNOWN;
    }
    
    ret = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (ret != AV_ERR_OK) {
        return AV_ERR_UNKNOWN;
    }
    return AV_ERR_OK;
}

int32_t Demuxer::ReadAudioSample(OH_AVBuffer *buffer, OH_AVCodecBufferAttr &attr)
{
    int32_t ret = OH_AVDemuxer_ReadSampleBuffer(demuxer_, audioTrackId_, buffer);
    if (ret != AV_ERR_OK) {
        return AV_ERR_UNKNOWN;
    }

    ret = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (ret != AV_ERR_OK) {
        return AV_ERR_UNKNOWN;
    }
    return AV_ERR_OK;
}

int32_t Demuxer::Release()
{
    if (source_ != nullptr) {
        OH_AVSource_Destroy(source_);
        source_ = nullptr;
    }
    if (demuxer_ != nullptr) {
        OH_AVDemuxer_Destroy(demuxer_);
        demuxer_ = nullptr;
    }
    return AV_ERR_OK;
}
