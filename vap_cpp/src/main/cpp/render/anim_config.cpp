/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include <fstream>
#include "common_const.h"
#include "log.h"
#include "anim_config.h"

using namespace CommonConst;

AnimConfig::~AnimConfig()
{
    if (srcMapPtr) {
        srcMapPtr->srcSMap.clear();
    }
    if (frameAllPtr) {
        frameAllPtr->frameAll.clear();
    }
};

void AnimConfig::ParseBoxHead(std::vector<char> &boxHead, BoxHead &head)
{
    int32_t length = ZERO;
    length |= (boxHead[ZERO] & ONE_EIGHT_HEX) << THREE_TIME_EIGHT;
    length |= (boxHead[ZERO] & ONE_EIGHT_HEX) << TWO_TIME_EIGHT;
    length |= (boxHead[TWO] & ONE_EIGHT_HEX) << EIGHT;
    length |= (boxHead[THREE] & ONE_EIGHT_HEX);
    head.length = length;
    head.type.assign(&boxHead[FOUR], &boxHead[FOUR] + FOUR);
}

int AnimConfig::GetJson(std::string &jsonStr, std::string &uri)
{
    std::ifstream file(uri, std::ios::binary);
    int ret = NEGATIVE_ONE;
    if (!file.is_open()) {
        return ret;
    }
    std::vector<char> buffer(EIGHT);
    while (file.read(buffer.data(), EIGHT)) {
        if (file.eof()) {
            break;
        }
        BoxHead boxHead;
        ParseBoxHead(buffer, boxHead);

        if (boxHead.type.compare("vapc") == ZERO) { // memcmp(boxHead.type.data(), "vapc", 4) == 0
            std::vector<char> cont(boxHead.length);
            file.read(cont.data(), boxHead.length);
            jsonStr.assign(cont.begin(), cont.end());
            ret = 0;
            break;
        }
        if (isH265_) {
            file.seekg(-FOUR, std::ios::cur);
        }
        if (boxHead.type.compare("moov") == 0) {
            file.seekg(boxHead.length - EIGHT, std::ios::cur);
        }
    }
    file.close();
    return ret;
}

bool AnimConfig::ParseJson(std::string uri, bool isNeedParseFrame)
{
    if (isInit) {
        return true;
    }
    std::string strJson;
    json jsonObj;
    if (GetJson(strJson, uri) == NEGATIVE_ONE) {
        return false;
    }
    if (strJson.empty()) {
        return false;
    }
    jsonObj = json::parse(strJson);
    if (jsonObj.find("info") == jsonObj.end()) {
        return false;
    }
    const auto &info = jsonObj.at("info");

    this->width = info.at("w").get<int>();
    this->height = info.at("h").get<int>();
    this->totalFrames = info.at("f").get<int>();
    this->videoWidth = info.at("videoW").get<int>();
    this->videoHeight = info.at("videoH").get<int>();
    this->orien = info.at("orien").get<Orien>();
    this->fps = info.at("fps").get<int>();
    this->isMix = info.at("isVapx").get<int>() == ONE;
    const auto& a = info.at("aFrame");
    this->alphaPointRect = std::move(PointRect(a.at(ZERO).get<int>(), a.at(ONE).get<int>(),
        a.at(TWO).get<int>(), a.at(THREE).get<int>()));
    const auto& c = info.at("rgbFrame");
    this->rgbPointRect = std::move(PointRect(c.at(ZERO).get<int>(), c.at(ONE).get<int>(),
        c.at(TWO).get<int>(), c.at(THREE).get<int>()));

    if (jsonObj.find("src") != jsonObj.end()) {
        srcMapPtr = std::make_shared<SrcMap>(jsonObj);
    } else {
        LOGW("json: not find src");
    }

    if (isNeedParseFrame && jsonObj.find("frame") != jsonObj.end()) {
        frameAllPtr = std::make_shared<FrameAll>(jsonObj);
    } else {
        LOGW("json: not find frame");
    }
    
    isInit = true;
    return true;
}

void AnimConfig::DefaultConfig(int32_t width, int32_t height)
{
    this->videoWidth = width;
    this->videoHeight = height;
    
    switch (defaultVideoMode) {
        case VIDEO_MODE_SPLIT_VERTICAL:
            this->width = width;
            this->height = height / TWO;
            this->alphaPointRect   = {ZERO, ZERO, this->width, this->height};
            this->rgbPointRect = {ZERO, this->height, this->width, this->height};
            break;
        case VIDEO_MODE_SPLIT_HORIZONTAL_REVERSE:
            this->width = width / TWO;
            this->height = height;
            this->rgbPointRect = {ZERO, ZERO, this->width, this->height};
            this->alphaPointRect = {this->width, ZERO, this->width, this->height};
            break;
        case VIDEO_MODE_SPLIT_VERTICAL_REVERSE:
            this->width = width;
            this->height = height / TWO;
            this->rgbPointRect = {ZERO, ZERO, this->width, this->height};
            this->alphaPointRect = {ZERO, this->height, this->width, this->height};
            break;
        default:
            this->width = width / TWO;
            this->height = height;
            this->alphaPointRect   = {ZERO, ZERO, this->width, this->height};
            this->rgbPointRect = {this->width, ZERO, this->width, this->height};
            break;
    }
}


void AnimConfig::GetSrc(std::map<std::string, Src> &src)
{
    if (!srcMapPtr) {
        LOGW("this file not have src prop");
    } else {
        src = srcMapPtr->srcSMap;
    }
}
