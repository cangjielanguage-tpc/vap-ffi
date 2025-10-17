/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_SRC_H
#define VAP_SRC_H

#include <map>
#include <string>

#include <nlohmann/json.hpp>
#include "log.h"

enum class SrcType {
    UNKNOWN,
    IMG,
    TXT,
};

enum class LoadType {
    UNKNOWN,
    NET,   // 网络加载的图片
    LOCAL, // 本地加载的图片
};

enum class FitType {
    FIT_XY,      // 按原始大小填充纹理
    CENTER_FULL, // 以纹理中心点放置
};

enum class Style {
    DEFAULT,
    BOLD, // 文字粗体
};

using json = nlohmann::json;

class Src {
public:
    Src(json jsonObj);
    static std::map<std::string, SrcType> string2SrcType;
    static SrcType stringToSrcTypeFunc(std::string str) { return string2SrcType[str]; }

    static std::map<std::string, LoadType> string2LoadType;
    static LoadType stringToLoadTypeFunc(std::string str) { return string2LoadType[str]; }

    static std::map<std::string, FitType> string2FitType;
    static FitType stringToFitTypeFunc(std::string str) { return string2FitType[str]; }

    static std::map<std::string, Style> string2Style;
    static Style stringToStyleFunc(std::string str) { return string2Style[str]; }

    std::string srcId;
    SrcType srcType = SrcType::UNKNOWN;
    LoadType loadType = LoadType::UNKNOWN;
    std::string srcTag;
    int32_t color = 0;
    Style style = Style::DEFAULT;
    int32_t w = 0;
    int32_t h = 0;
    FitType fitType = FitType::FIT_XY;

    int32_t drawWidth = 0;
    int32_t drawHeight = 0;
    std::string txt;
    int32_t srcTextureId = 0;

    void PrintInfo();
};

class SrcMap {
public:
    SrcMap(json jsonObj);
    SrcMap() {}
    
    std::map<std::string, Src> srcSMap;
};
#endif // VAP_SRC_H
