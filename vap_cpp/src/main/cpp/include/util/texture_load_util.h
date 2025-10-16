/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_TEXTURE_LOADUTIL_H
#define VAP_TEXTURE_LOADUTIL_H

#include <GLES3/gl32.h>
#include <stdint.h>
#include <vector>

class BitMap {
public:
    std::vector<uint8_t> pixelsData;
    int32_t imgWidth;
    int32_t imgHeight;
};

class TextureLoadUtil {
public:
    static TextureLoadUtil &GetInstance()
    {
        static TextureLoadUtil instance;
        return instance;
    }
    void loadTexture(BitMap bitmap, GLuint *textures);
};
#endif // VAP_TEXTURE_LOADUTIL_H
