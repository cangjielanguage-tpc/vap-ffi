/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "frame.h"

#include <algorithm>
#include "log.h"
#include "common_const.h"

using namespace CommonConst;

Frame::Frame(json jsonFrame)
{
//    srcId = jsonFrame.at("srcId").get<std::string>();
    auto srcIdIt = jsonFrame.find("srcId");
    if (srcIdIt != jsonFrame.end()) {
        srcId = srcIdIt->get<std::string>();
    } else {
        LOGE("Frame: missing srcId");
    }
    
//    z = jsonFrame.at("z").get<int>();
    auto zIt = jsonFrame.find("z");
    if (zIt != jsonFrame.end()) {
        z = zIt->get<int>();
    } else {
        LOGE("Frame: missing z");
    }

//    const auto& frameRect = jsonFrame.at("frame");
//    frame = std::move(PointRect(frameRect.at(ZERO).get<int>(), frameRect.at(ONE).get<int>(),
//        frameRect.at(TWO).get<int>(), frameRect.at(THREE).get<int>()));
//    
//    const auto& mFrameRect = jsonFrame.at("mFrame");
//    mFrame = std::move(PointRect(mFrameRect.at(ZERO).get<int>(), mFrameRect.at(ONE).get<int>(),
//        mFrameRect.at(TWO).get<int>(), mFrameRect.at(THREE).get<int>()));
//    
//    mt = jsonFrame.at("mt").get<int>();
//    json j(frameRect);
    
    auto frameRectIt = jsonFrame.find("frame");
    if (frameRectIt != jsonFrame.end()) {
        const auto& frameRect = *frameRectIt;
        frame = PointRect(
            frameRect[ZERO].get<int>(), frameRect[ONE].get<int>(),
            frameRect[TWO].get<int>(), frameRect[THREE].get<int>()
        );
    } else {
        LOGE("Frame: missing frame");
    }

    auto mFrameRectIt = jsonFrame.find("mFrame");
    if (mFrameRectIt != jsonFrame.end()) {
        const auto& mFrameRect = *mFrameRectIt;
        mFrame = PointRect(
            mFrameRect[ZERO].get<int>(), mFrameRect[ONE].get<int>(),
            mFrameRect[TWO].get<int>(), mFrameRect[THREE].get<int>()
        );
    } else {
        LOGE("Frame: missing mFrame");
    }

    auto mtIt = jsonFrame.find("mt");
    if (mtIt != jsonFrame.end()) {
        mt = mtIt->get<int>();
    } else {
        LOGE("Frame: missing mt");
    }
}

FrameSet::FrameSet(json jsonObj)
{
//    index = jsonObj.at("i").get<int>();
//    auto objJsonArray = jsonObj.at("obj");
//    size_t objLen = objJsonArray.size();
//    for (int i = 0; i < objLen; i++) {
//        auto frameJson = objJsonArray[i];
//        Frame frame = Frame(frameJson);
//        frames.push_back(frame);
//    }
//    std::sort(frames.begin(), frames.end());
    auto indexIt = jsonObj.find("i");
    if (indexIt != jsonObj.end()) {
        index = indexIt->get<int>();
    } else {
        LOGE("FrameSet: missing i");
    }

    auto objJsonArrayIt = jsonObj.find("obj");
    if (objJsonArrayIt == jsonObj.end()) {
        LOGE("FrameSet: missing obj");
        return;
    }

    const auto& objJsonArray = *objJsonArrayIt;
    size_t objLen = objJsonArray.size();
    frames.reserve(objLen); // 预分配内存，避免多次扩容
    for (size_t i = 0; i < objLen; ++i) { // 用size_t避免类型转换
        frames.emplace_back(objJsonArray[i]); // 直接构造，减少拷贝
    }
    std::sort(frames.begin(), frames.end());
}

FrameAll::FrameAll(json jsonObj)
{
    auto frameJsonArray = jsonObj.at("frame");
    size_t frameLen = frameJsonArray.size();
    LOGD("enter FrameAll frame size: %{public}lu", frameLen);
    for (int i = 0; i < frameLen; i++) {
        auto frameSetJson = frameJsonArray[i];
        auto frameSet = FrameSet(frameSetJson);
        frameAll.insert(std::make_pair(frameSet.index, frameSet));
    }
}