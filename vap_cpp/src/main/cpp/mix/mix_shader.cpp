/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "mix_shader.h"

MixShader::MixShader()
{
    m_program = ShaderUtil::CreateProgram(VERTEX, FRAGMENT);
    m_uTextureSrcUnitLocation = glGetUniformLocation(m_program, U_TEXTURE_SRC_UNIT);
    m_uTextureMaskUnitLocation = glGetUniformLocation(m_program, U_TEXTURE_MASK_UNIT);
    m_uIsFillLocation = glGetUniformLocation(m_program, U_IS_FILL);
    m_uColorLocation = glGetUniformLocation(m_program, U_COLOR);

    m_aPositionLocation = glGetAttribLocation(m_program, A_POSITION);
    m_aTextureSrcCoordinatesLocation = glGetAttribLocation(m_program, A_TEXTURE_SRC_COORDINATES);
    m_aTextureMaskCoordinatesLocation = glGetAttribLocation(m_program, A_TEXTURE_MASK_COORDINATES);
}

void MixShader::UseProgram() { glUseProgram(m_program); }