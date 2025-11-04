/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "mask_config.h"

MaskConfig::MaskConfig(int32_t width, int32_t height)
{
    width_ = width;
    maskTexId_ = height;
}

MaskConfig::~MaskConfig()
{
    if (maskTexId_) {
        glDeleteTextures(1, &maskTexId_);
    }
}

int32_t MaskConfig::GetMaskTexId()
{
    return maskTexId_;
}

int32_t MaskConfig::UpdateMaskTex(BitMap alphaMaskBitmap)
{
    TextureLoadUtil::GetInstance().loadTexture(alphaMaskBitmap, &maskTexId_);
    return maskTexId_;
}