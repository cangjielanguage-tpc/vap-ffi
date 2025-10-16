/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "mask_shader.h"

MaskShader::MaskShader(bool edgeBlurBoolean)
{
    if (edgeBlurBoolean) {
        m_program = ShaderUtil::CreateProgram(MASK_VERTEX, FRAGMENT_BLUR_EDGE);
    } else {
        m_program = ShaderUtil::CreateProgram(MASK_VERTEX, FRAGMENT_NO_BLUR_EDGE);
    }
    m_uTextureMaskUnitLocation = glGetUniformLocation(m_program, U_TEXTURE_ALPHA_MASK_UNIT);
    m_aPositionLocation = glGetAttribLocation(m_program, A_POSITION);
    m_aTextureMaskCoordinatesLocation = glGetAttribLocation(m_program, A_TEXTURE_MASK_COORDINATES);
}

void MaskShader::UseProgram()
{
    glUseProgram(m_program);
}
