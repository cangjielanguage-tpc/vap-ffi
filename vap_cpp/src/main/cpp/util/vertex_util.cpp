/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "vertex_util.h"
#include "log.h"
#include "common_const.h"

using namespace CommonConst;

void VertexUtil::Create(int32_t width, int32_t height, const PointRect &rect, float *array)
{
    if (width > ZERO && height > ZERO) {
        array[ZERO] = SwitchX(static_cast<float>(rect.x) / width);
        array[ONE] = SwitchY(static_cast<float>(rect.y) / height);
        array[TWO] = SwitchX(static_cast<float>(rect.x) / width);
        array[THREE] = SwitchY((static_cast<float>(rect.y) + rect.h) / height);
        array[FOUR] = SwitchX((static_cast<float>(rect.x) + rect.w) / width);
        array[FIVE] = SwitchY(static_cast<float>(rect.y) / height);
        array[SIX] = SwitchX((static_cast<float>(rect.x) + rect.w) / width);
        array[SEVEN] = SwitchY((static_cast<float>(rect.y) + rect.h) / height);
    }
}

float VertexUtil::SwitchX(float x)
{
    return x * 2.0f - 1.0f;
}

float VertexUtil::SwitchY(float y)
{
    return ((y * 2.0f - 2.0f) * -1.0f) - 1.0f;
}

void VertexUtil::EnableVertexAttrib(GLuint index)
{
    GLuint buffer;
    glGenBuffers(ONE, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*floatBuffer) * EIGHT, floatBuffer, GL_STATIC_DRAW);
    glVertexAttribPointer(index, TWO, GL_FLOAT, GL_FALSE, sizeof(float) * TWO, (void *)ZERO);
    glEnableVertexAttribArray(index);
}

void VertexUtil::SetArray(float *array)
{
    floatBuffer = array;
}