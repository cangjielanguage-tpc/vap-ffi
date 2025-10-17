/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_MASK_RENDER_H
#define VAP_MASK_RENDER_H

#include "mask_config.h"
#include "mask_shader.h"
#include "vertex_util.h"

class MaskRender {
public:
    MaskRender(bool edgeBlur);
    void RenderFrame(MaskConfig maskConfig);
    
public:
    std::unique_ptr<MaskShader> m_maskShader;
    VertexUtil m_vertexArray;
    
private:
    VertexUtil m_maskArray;
};

#endif // VAP_MASK_RENDER_H
