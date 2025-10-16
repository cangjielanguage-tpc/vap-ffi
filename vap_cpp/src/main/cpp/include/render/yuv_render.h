/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VAP_RENDER_H
#define VAP_RENDER_H

#include <GLES3/gl31.h>
#include <memory>
#include <vector>
#include "anim_config.h"
#include "log.h"

class YuvRender {
public:
    YuvRender(std::string uri)
    {
        InitRender(uri);
    }
    void InitRender(std::string uri);
    void SetYUVData(int32_t width, int32_t height, const std::vector<unsigned char> &y,
        const std::vector<unsigned char> &u, const std::vector<unsigned char> &v);
    void Draw();
    void SetAnimConfig(std::shared_ptr<AnimConfig> animConfig);
    void RenderFrame();
    void ClearFrame();
    void ReleaseTexture();
    GLuint GetExternalTexture();

    VertexUtil vertex_;
    VertexUtil alpha_;
    VertexUtil rgb_;
    
    int widthYUV_ = 0;
    int heightYUV_ = 0;
    std::vector<unsigned char> y_;
    std::vector<unsigned char> u_;
    std::vector<unsigned char> v_;
    GLuint shaderProgram_ = 0;
    GLuint VAO_;
private:
    GLint avPosition_ = 0;
    GLint rgbPosition_ = 0;
    GLint alphaPosition_ = 0;
    
    GLint samplerY_ = 0;
    GLint samplerU_ = 0;
    GLint samplerV_ = 0;
    
    GLint convertMatrixUniform_ = 0;
    GLint convertOffsetUniform_ = 0;
    
    GLuint textureId_[3];
    GLint unpackAlign_ = 4;
    
    float YUV_OFFSET[3] = {
        0.0f, -0.501960814f, -0.501960814f
    };
    
    float YUV_MATRIX[9] = {
        1.0f, 1.0f, 1.0f,
        0.0f, -0.3441f, 1.772f,
        1.402f, -0.7141f, 0.0f
    };
};

#endif // VAP_RENDER_H
