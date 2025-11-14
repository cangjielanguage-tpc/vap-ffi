/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_MASK_CONFIG_H
#define VAP_MASK_CONFIG_H

#include <tuple>

#include "texture_load_util.h"
#include "vertex_util.h"

class MaskConfig {
public:
    MaskConfig(int32_t width, int32_t height);
    ~MaskConfig();
    
    int32_t GetMaskTexId();
    int32_t UpdateMaskTex(BitMap alphaMaskBitmap);

public:
    std::tuple<PointRect, RefVec2> maskTexPair_ =
        std::move(std::make_tuple(PointRect(0, 0, 0, 0), RefVec2(0, 0)));

    std::tuple<PointRect, RefVec2> maskPositionPair_ =
        std::move(std::make_tuple(PointRect(0, 0, 0, 0), RefVec2(0, 0)));

    int32_t width_; // animConfig width
    int32_t height_;
private:
    GLuint maskTexId_ = 0;
};

#endif // VAP_MASK_CONFIG_H
