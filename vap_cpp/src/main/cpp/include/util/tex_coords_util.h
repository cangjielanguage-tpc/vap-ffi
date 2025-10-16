/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef VAP_TEX_COORDS_UTIL_H
#define VAP_TEX_COORDS_UTIL_H

#include <stdint.h>
#include <sys/stat.h>
#include "vertex_util.h"

class TexCoordsUtil {
public:
    static void Create(int32_t width, int32_t height, PointRect rect, float *array);
    static void Rotate90(float *array);
};

#endif // VAP_TEX_COORDS_UTIL_H
