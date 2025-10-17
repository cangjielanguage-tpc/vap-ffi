/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_FRAME_H
#define VAP_FRAME_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include "vertex_util.h"

using json = nlohmann::json;

class Frame {
public:
    Frame(json jsonObj);
    
    std::string srcId = "";
    int32_t z = 0;
    PointRect frame;
    PointRect mFrame;
    int32_t mt = 0; // 遮罩旋转角度v2 版本只支持 0 与 90度

    bool operator<(const Frame &f) const { return z < f.z; }
    void PrintInfo();
};

class FrameSet {
public:
    FrameSet(json jsonObj);
    
    std::vector<Frame> frames;
    int index;
};

class FrameAll {
public:
    FrameAll(json jsonObj);
    
    std::map<int, FrameSet> frameAll;
};
#endif // VAP_FRAME_H
