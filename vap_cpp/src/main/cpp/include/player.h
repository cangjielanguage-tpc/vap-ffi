/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_PLAYER_H
#define VAP_PLAYER_H

#include <mutex>
#include <memory>
#include <atomic>
#include <thread>
#include <unistd.h>
#include "audio_decoder.h"
#include "video_decoder.h"
#include "demuxer.h"
#include "egl_core.h"
#include "data_info.h"

enum class CallbackType {
    UNKNOWN = -1,
    PLAY_DONE,
    STATE_CHANGE,
    CLICK,
};

typedef void(*callbackcj)();
static constexpr int64_t MICROSECOND = 1000000;
static constexpr int32_t DEFAULT_FRAME_RATE = 30;
static constexpr int32_t AUDIO_SLEEP_TIME = 100;
static constexpr int32_t AUDIO_CACHE = 5;

class Player {
public:
    Player() {};
    ~Player();
    int32_t Init(VAPInfo &info);
    int32_t Start();
    void PauseAndResume();
    void Stop();
    void SetLoop(int32_t loops = 1) { loops_ = loops; }
    void SetFitType(VideoFitType fitType) { fitType_ = fitType; }
    void SetSpeed(float speed) { speed_ = speed; }
    void SetVideoMode(VideoMode videoMode);
    bool IsRunning() { return isStarted_; }
    void CallBackJS(VapState state, CallbackType type, int32_t err = -1);
    void StartRelease();
    void NoticeDestroyed()
    {
        isContainerDestroy_ = true;
        Stop();
    };
    typedef void(*stateCallback)(int64_t, int64_t);
    typedef void(*stateCallback1)();
    void SetCallback(CallbackType type, stateCallback1 callbackw, void (*callbackFn)(void *context));
    std::unique_ptr<EGLCore> eglCore_ = nullptr;
private:
    void (*callbackAll_)(void *context) = nullptr;
    std::mutex callbackRefsMutex_;
    std::map<CallbackType, callbackcj> callbackRefs_;
    bool isContainerDestroy_ = false;
    bool isSetVideoMode_ = false;
    VideoMode defaultVideoMode_ = VIDEO_MODE_SPLIT_HORIZONTAL;
    JSAnimConfig jsAnimConfig_;
    void InitJSAnimConfig();
    int32_t loops_ = 1;
    float speed_ = 1.0;
    std::mutex pauseMutex_;
    std::condition_variable pauseCond_;
    bool isPause_{false};
    void InitAudioPlayer(AudioInitData audioInitData);
    OH_AudioRenderer *audioRenderer_ = nullptr;
    OH_AudioStreamBuilder *builder_ = nullptr;
    void Release();
    void ReleaseAudio();
    void DecInputThread();
    void DecOutputThread();
    void DecAudioInputThread();
    void DecAudioOutputThread();
    void RenderThread();
    int32_t InitAudio();
    void InitControlSignal();
    void ResetControlSignal();
    void GetBufferData(CodecBufferInfo bufferInfo);
    void GetPCMData(AudioCodecBufferInfo bufferInfo);
    std::mutex renderMutex_;
    std::queue<std::vector<uint8_t>> renderQueue_;
    std::unique_ptr<std::thread> renderThread_ = nullptr;
    std::condition_variable renderCond_;
    std::unique_ptr<Demuxer> demuxer_ = nullptr;
    std::unique_ptr<VideoDecoder> videoDecoder_ = nullptr;
    std::unique_ptr<AudioDecoder> audioDecoder_ = nullptr;
    ADecSignal *audioSignal_ = nullptr;
    VDecSignal *signal_ = nullptr;
    std::atomic<bool> isStop_{false};
    std::atomic<bool> isStarted_{false};
    std::atomic<bool> isReleased_{false};
    std::atomic<bool> isVideoEndOfFile_{false};
    std::atomic<bool> isVideoEnd_{false};
    std::atomic<bool> isAudioEnd_{false};
    std::unique_ptr<std::thread> decAudioInputThread_ = nullptr;
    std::unique_ptr<std::thread> decAudioOutputThread_ = nullptr;
    std::mutex mutex_;
    std::unique_ptr<std::thread> decInputThread_ = nullptr;
    std::unique_ptr<std::thread> decOutputThread_ = nullptr;
    VAPInfo sampleInfo_;
    int32_t stride_ = 0;
    int32_t sliceHeight_ = 0;
    int32_t frameCurIdx_ = 0;
    int32_t renderFrameCurIdx_ = 0;
    VideoFitType fitType_ = VideoFitType::FIT_XY;
};
#endif // VAP_PLAYER_H