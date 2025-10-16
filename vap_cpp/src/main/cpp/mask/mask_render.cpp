/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "mask/mask_render.h"

#include "util/log.h"

#include <GLES2/gl2ext.h>
#include "common_const.h"

using namespace CommonConst;

void MaskRender::RenderFrame(MaskConfig maskConfig)
{
    auto maskTexId = maskConfig.GetMaskTexId();
    if (maskTexId <= 0) {
    }
    
    auto texTuple = maskConfig.maskTexPair_;
    auto maskTexRect = std::get<0>(texTuple);
    auto maskTexRefVec2 = std::get<1>(texTuple);
    
    auto positionTuple = maskConfig.maskPositionPair_;
    PointRect maskPositionRect;
    if (std::get<0>(positionTuple).w != 0 && std::get<0>(positionTuple).h != 0) {
        maskPositionRect = std::get<0>(positionTuple);
    } else {
        maskPositionRect = PointRect(0, 0, maskConfig.m_width, maskConfig.m_height);
    }

    RefVec2 maskPositionRefVec2;
    if (std::get<1>(positionTuple).w != 0 && std::get<1>(positionTuple).h !=0) {
        maskPositionRefVec2 = std::get<1>(positionTuple);
    } else {
        maskPositionRefVec2 = RefVec2(maskConfig.m_width, maskConfig.m_height);
    }
    
    m_maskShader->UseProgram();
    m_vertexArray.Create(maskPositionRefVec2.w, maskPositionRefVec2.h, maskPositionRect, m_vertexArray.array.get());
    m_vertexArray.SetArray(m_vertexArray.array.get());
    m_vertexArray.EnableVertexAttrib(m_maskShader->m_aPositionLocation);
    
    m_maskArray.Create(
        maskTexRefVec2.w,
        maskTexRefVec2.h,
        maskTexRect,
        m_maskArray.array.get()
    );
    m_maskArray.SetArray(m_maskArray.array.get());
    m_maskArray.EnableVertexAttrib(m_maskShader->m_aTextureMaskCoordinatesLocation);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(m_maskShader->m_uTextureMaskUnitLocation, 0);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_SRC_ALPHA, GL_ZERO, GL_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, FOUR);
    glDisable(GL_BLEND);
}

MaskRender::MaskRender(bool edgeBlur)
{
    m_maskShader = std::make_unique<class MaskShader>(edgeBlur);
    glDisable(GL_DEPTH_TEST);
}