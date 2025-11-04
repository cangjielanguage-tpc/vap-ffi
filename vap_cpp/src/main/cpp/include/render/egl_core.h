/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NATIVE_XCOMPONENT_EGL_CORE_H
#define NATIVE_XCOMPONENT_EGL_CORE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "mask_render.h"
#include "mix_render.h"
#include "data_info.h"
#include "yuv_render.h"

struct RenderData {
    std::vector<uint8_t> data;
    int32_t stride;
    int32_t sliceHeight;
    int32_t frameCurIdx;
};

class EGLCore {
public:
    explicit EGLCore(){};
    ~EGLCore() {}
    bool EglContextInit(void* window);

    bool EglContextInit1(EGLNativeWindowType window, int width, int height, std::string uri,
        std::map<std::string, MixInputData> &iptData);

    bool CreateEnvironment();
    void Draw(int& hasDraw);
    void Background();
    void ChangeColor(int& hasChangeColor);
    void Release();
//    void UpdateSize(int width, int height);
    
    void SetFitType(VideoFitType fitType)
    {
        m_fitType = fitType;
    }
    void SetVideoMode(VideoMode videoMode)
    {
        m_defaultVideoMode = videoMode;
        if (m_animConfig) {
            m_animConfig->isDefaultConfig = true;
        }
    }
    void SetVideoSize(int32_t width, int32_t height, std::string mime)
    {
        m_videoWidth = width;
        m_videoHeight = height;
        m_isH265 = mime == MIME_VIDEO_HEVC;
    }
    
    void Render(RenderData &renderData);
    bool SeparateYUV(const std::vector<uint8_t> &data,
        int alignWidth, int alignHeight, int videoWidth, int videoHeight);
    bool FinishLoad();
    void GetCurrentClickTxt(int32_t eventX, int32_t eventY, std::string &clickRes);
    
    void UpdateSize(int width, int height)
    {
        LOGD("enter UpdateSize %{public}d * %{public}d ", width, height);
        m_width = width;
        m_height = height;
        InitFitConfig();
    }
    
    std::shared_ptr<AnimConfig> m_animConfig;

private:
    GLuint LoadShader(GLenum type, const char * shaderSrc);
    GLuint CreateProgram(const char* vertexShader, const char* fragShader);
    GLint PrepareDraw();
    bool ExecuteDraw(GLint position, const GLfloat* color, const GLfloat shapeVertices[], unsigned long vertSize);
    bool ExecuteDrawStar(GLint position, const GLfloat* color, const GLfloat shapeVertices[], unsigned long vertSize);
    bool ExecuteDrawNewStar(GLint position, const GLfloat* color, const GLfloat shapeVertices[], unsigned long vertSize);
    void Rotate2d(GLfloat centerX, GLfloat centerY, GLfloat* rotateX, GLfloat* rotateY, GLfloat theta);
    bool FinishDraw();

    VideoMode m_defaultVideoMode = VIDEO_MODE_SPLIT_HORIZONTAL;
    EGLConfig ChooseConfig();
    void GetAttributes(std::vector<int> &array);
    EGLContext CreateContext(EGLDisplay eglDisplay, EGLConfig eglConfig);
    void MixRenderFrame(int32_t frameCurIdx);
    void InitFitConfig();
    void FitCenter();
    void CenterCrop();
    void FitXY();

private:
    EGLNativeWindowType m_eglWindow;
    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;
    EGLContext m_eglContext = EGL_NO_CONTEXT;
    GLuint m_program;
    bool m_flag = false;
    int m_width;
    int m_height;
    GLfloat m_widthPercent;
    unsigned char *buf[3] = {0};
    std::unique_ptr<class YuvRender> m_render = nullptr;
    std::unique_ptr<class MixRender> m_mixRender = nullptr;
    std::unique_ptr<MaskRender> m_maskRender = nullptr;
    
    int32_t m_frameCurIdx;
    VideoFitType m_fitType = VideoFitType::FIT_XY;
    int32_t m_viewportWidth = 0;
    int32_t m_viewportHeight = 0;
    int32_t m_viewportX = 0;
    int32_t m_viewportY = 0;
    
    int32_t m_videoWidth = 0;
    int32_t m_videoHeight = 0;
    bool m_isH265 = false;
};
#endif // NATIVE_XCOMPONENT_EGL_CORE_H
