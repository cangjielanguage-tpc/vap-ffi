/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "tex_coords_util.h"
#include "common_const.h"

using namespace CommonConst;

void TexCoordsUtil::Create(int32_t width, int32_t height, PointRect rect, float *array)
{
    if (width > ZERO && height > ZERO) {
        // x0
        array[ZERO] = static_cast<float>(rect.x) / width;
        // y0
        array[ONE] = static_cast<float>(rect.y) / height;
    
        // x1
        array[TWO] = static_cast<float>(rect.x) / width;
        // y1
        array[THREE] = (static_cast<float>(rect.y) + rect.h) / height;
    
        // x2
        array[FOUR] = (static_cast<float>(rect.x) + rect.w) / width;
        // y2
        array[FIVE] = static_cast<float>(rect.y) / height;
    
        // x3
        array[SIX] = (static_cast<float>(rect.x) + rect.w) / width;
        // y3
        array[SEVEN] = (static_cast<float>(rect.y) + rect.h) / height;
    }
}

void TexCoordsUtil::Rotate90(float *array)
{
    // 0->2 1->0 3->1 2->3
    auto tx = array[ZERO];
    auto ty = array[ONE];

    // 1->0
    array[ZERO] = array[TWO];
    array[ONE] = array[THREE];

    // 3->1
    array[TWO] = array[SIX];
    array[THREE] = array[SEVEN];

    // 2->3
    array[SIX] = array[FOUR];
    array[SEVEN] = array[FIVE];

    // 0->2
    array[FOUR] = tx;
    array[FIVE] = ty;
}