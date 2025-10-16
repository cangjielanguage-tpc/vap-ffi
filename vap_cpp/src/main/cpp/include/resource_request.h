/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_RESOURCE_REQUEST_H
#define VAP_RESOURCE_REQUEST_H

#include <bits/alltypes.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <native_drawing/drawing_text_typography.h>
#include <native_drawing/drawing_types.h>
#include <vector>

#include "mix/src.h"

struct ColorARGB {
    uint32_t alpha = 0xff;
    uint32_t red = 0xff;
    uint32_t green = 0xf1;
    uint32_t blue = 0xae;
};

struct TextOption {
    const char *text;
    int32_t height;
    int32_t width;
    
    ColorARGB color;
    OH_Drawing_TextAlign textAlign;
    OH_Drawing_FontWeight fontWeight;
};
struct ImageOption {
    const char *uri;
    int32_t height;
    int32_t width;
    FitType fixType;
};
class ResourceRequest {
public:
    ResourceRequest() = default;
    ~ResourceRequest() { Release();};
    void Create();
    void DrawText(TextOption &txtOpt);
    void SetWidth(uint64_t width = 300);
    void SetHeight(uint64_t height = 50);
    void GetData(std::vector<uint8_t> &data);
    void Release();
    
    void FetchText(std::vector<uint8_t> &data, TextOption &txtOpt);
    void FetchImg(std::vector<uint8_t> &data, ImageOption &imgOpt);
private:
    uint64_t m_width = 300;
    uint64_t m_height = 50;

    OH_Drawing_Bitmap *cBitmap_ = nullptr;
    OH_Drawing_Canvas *cCanvas_ = nullptr;

    OH_ImageSourceNative *source_ = nullptr;
    OH_PixelmapNative *resPixMap_ = nullptr;

    static constexpr double DEFAULT_FONT_SIZE = 31.0;
};
#endif // VAP_RESOURCE_REQUEST_H
