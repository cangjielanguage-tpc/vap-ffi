/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "egl_core.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <GLES3/gl3.h>
#include <cmath>
#include <cstdio>
#include <hilog/log.h>
#include "../common/common.h"
#include "plugin_render.h"
#include <cstddef>
#include <memory>
#include <fstream>
#include <vector>
#include "mask_config.h"
#include "mask_render.h"
#include "util/log.h"
#include <cstdint>
#include <misc/fastrpc.h>
#include "common_const.h"

#define OHOSCHARPTR char*

using namespace CommonConst;
constexpr int32_t NUM_4 = 4;

/**
 * Vertex shader.
 */
const char VERTEX_SHADER[] = "#version 300 es\n"
                             "layout(location = 0) in vec4 a_position;\n"
                             "layout(location = 1) in vec4 a_color;   \n"
                             "out vec4 v_color;                       \n"
                             "void main()                             \n"
                             "{                                       \n"
                             "   gl_Position = a_position;            \n"
                             "   v_color = a_color;                   \n"
                             "}                                       \n";

/**
 * Fragment shader.
 */
const char FRAGMENT_SHADER[] = "#version 300 es\n"
                               "precision mediump float;                  \n"
                               "in vec4 v_color;                          \n"
                               "out vec4 fragColor;                       \n"
                               "void main()                               \n"
                               "{                                         \n"
                               "   fragColor = v_color;                   \n"
                               "}                                         \n";

/**
 * Background color #f4f4f4.
 */
const GLfloat BACKGROUND_COLOR[] = {244.0f / 255, 244.0f / 255, 244.0f / 255, 1.0f};

/**
 * Draw color #7E8FFB.
 */
const GLfloat DRAW_COLOR[] = {126.0f / 255, 143.0f / 255, 251.0f / 255, 1.0f};

/**
 * Change color #92D6CC.
 */
const GLfloat CHANGE_COLOR[] = {146.0f / 255, 214.0f / 255, 204.0f / 255, 1.0f};

/**
 * Background area.
 */
const GLfloat BACKGROUND_RECTANGLE_VERTICES[] = {
    -1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, -1.0f,
    -1.0f, -1.0f};

/**
 * Get context parameter count.
 */
const size_t GET_CONTEXT_PARAM_CNT = 1;

/**
 * Fifty percent.
 */
const float FIFTY_PERCENT = 0.5;

/**
 * Pointer size.
 */
const GLint POINTER_SIZE = 2;

/**
 * Triangle fan size.
 */
const GLsizei TRIANGLE_FAN_SIZE = 4;

/**
 * Egl red size default.
 */
const int EGL_RED_SIZE_DEFAULT = 8;

/**
 * Egl green size default.
 */
const int EGL_GREEN_SIZE_DEFAULT = 8;

/**
 * Egl blue size default.
 */
const int EGL_BLUE_SIZE_DEFAULT = 8;

/**
 * Egl alpha size default.
 */
const int EGL_ALPHA_SIZE_DEFAULT = 8;

/**
 * Default x position.
 */
const int DEFAULT_X_POSITION = 0;

/**
 * Default y position.
 */
const int DEFAULT_Y_POSITION = 0;

/**
 * Gl red default.
 */
const GLfloat GL_RED_DEFAULT = 0.0;

/**
 * Gl green default.
 */
const GLfloat GL_GREEN_DEFAULT = 0.0;

/**
 * Gl blue default.
 */
const GLfloat GL_BLUE_DEFAULT = 0.0;

/**
 * Gl alpha default.
 */
const GLfloat GL_ALPHA_DEFAULT = 1.0;

/**
 * Program error.
 */
const GLuint PROGRAM_ERROR = 0;

/**
 * Shape vertices size.
 */
const int SHAPE_VERTICES_SIZE = 8;

/**
 * Position handle name.
 */
const char POSITION_NAME[] = "a_position";

/**
 * Position error.
 */
const GLint POSITION_ERROR = -1;

/**
 * Config attribute list.
 */
const EGLint ATTRIB_LIST[] = {
    // Key,value.
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, EGL_RED_SIZE_DEFAULT,
    EGL_GREEN_SIZE, EGL_GREEN_SIZE_DEFAULT,
    EGL_BLUE_SIZE, EGL_BLUE_SIZE_DEFAULT,
    EGL_ALPHA_SIZE, EGL_ALPHA_SIZE_DEFAULT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    // End.
    EGL_NONE};

/**
 * Context attributes.
 */
const EGLint CONTEXT_ATTRIBS[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE};

bool EGLCore::EglContextInit(void* window)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore", "EglContextInit execute");
    m_eglWindow = static_cast<EGLNativeWindowType>(window);

    // Init display.
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_eglDisplay == EGL_NO_DISPLAY) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglGetDisplay: unable to get EGL display");
        return false;
    }

    EGLint majorVersion;
    EGLint minorVersion;
    if (!eglInitialize(m_eglDisplay, &majorVersion, &minorVersion)) {
        OH_LOG_Print(
            LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglInitialize: unable to get initialize EGL display");
        return false;
    }

    // Select configuration.
    const EGLint maxConfigSize = 1;
    EGLint numConfigs;
    if (!eglChooseConfig(m_eglDisplay, ATTRIB_LIST, &m_eglConfig, maxConfigSize, &numConfigs)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglChooseConfig: unable to choose configs");
        return false;
    }
    return CreateEnvironment();
}

bool EGLCore::EglContextInit(EGLNativeWindowType window, int width, int height, std::string uri,
    std::map<std::string, MixInputData> &iptData)
{
    LOGD("EglContextInit execute");
    if ((nullptr == window) || (ZERO >= width) || (ZERO >= height)) {
        LOGE("EglContextInit: param error");
        return false;
    }

    m_width = width;
    m_height = height;

    m_animConfig = std::make_shared<AnimConfig>();
    m_animConfig->defaultVideoMode = m_defaultVideoMode;
    if (m_animConfig->isDefaultConfig || m_animConfig->ParseJson(uri) == false) {
        m_animConfig->DefaultConfig(m_videoWidth, m_videoHeight);
    }
    InitFitConfig();

    if (0 < m_width) {
        m_widthPercent = FIFTY_PERCENT * m_height / m_width;
    }
    m_eglWindow = window;

    // Init display.
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGL_NO_DISPLAY == m_eglDisplay) {
        LOGE("eglGetDisplay: unable to get EGL display");
        return false;
    }

    EGLint majorVersion;
    EGLint minorVersion;
    if (!eglInitialize(m_eglDisplay, &majorVersion, &minorVersion)) {
        LOGE("eglInitialize: unable to get initialize EGL display");
        return false;
    }

    // Select configuration.
    m_eglConfig = ChooseConfig();
    if (m_eglConfig == nullptr) {
        LOGE("GLContextInit config ERROR");
        return false;
    }
    auto ret = CreateEnvironment();

    m_render = std::make_unique<class YuvRender>(uri);
    m_render->SetAnimConfig(m_animConfig);

    if (m_animConfig->srcMapPtr) {
        m_mixRender = std::make_unique<class MixRender>(iptData);
        m_mixRender->SetAnimConfig(m_animConfig);
    }
    return ret;
}

bool EGLCore::CreateEnvironment()
{
    // Create surface.
    if (m_eglWindow == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglWindow_ is null");
        return false;
    }
    if (m_eglDisplay == EGL_NO_DISPLAY || m_eglConfig == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "m_eglDisplay is null");
        return false;
    }
    
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_eglWindow, NULL);
    if (m_eglSurface == nullptr) {
        OH_LOG_Print(
            LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglCreateWindowSurface: unable to create surface");
        return false;
    }
    // Create context.
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglMakeCurrent failed");
        return false;
    }
    // Create program.
    m_program = CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (m_program == PROGRAM_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "CreateProgram: unable to create program");
        return false;
    }
    return true;
}

void EGLCore::Background()
{
    GLint position = PrepareDraw();
    if (position == POSITION_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Background get position failed");
        return;
    }

    if (!ExecuteDraw(position, BACKGROUND_COLOR,
                     BACKGROUND_RECTANGLE_VERTICES, sizeof(BACKGROUND_RECTANGLE_VERTICES))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Background execute draw failed");
        return;
    }

    if (!FinishDraw()) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Background FinishDraw failed");
        return;
    }
}

void EGLCore::Draw(int& hasDraw)
{
    m_flag = false;
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore", "Draw");
    GLint position = PrepareDraw();
    if (position == POSITION_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw get position failed");
        return;
    }

    if (!ExecuteDraw(position, BACKGROUND_COLOR,
                     BACKGROUND_RECTANGLE_VERTICES, sizeof(BACKGROUND_RECTANGLE_VERTICES))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw background failed");
        return;
    }

    // Divided into five quadrilaterals and calculate one of the quadrilateral's Vertices
    GLfloat rotateX = 0;
    GLfloat rotateY = FIFTY_PERCENT * m_height;
    GLfloat centerX = 0;
    // Convert DEG(54° & 18°) to RAD
    GLfloat centerY = -rotateY * (M_PI / 180 * 54) * (M_PI / 180 * 18);
    // Convert DEG(18°) to RAD
    GLfloat leftX = -rotateY * (M_PI / 180 * 18);
    GLfloat leftY = 0;
    // Convert DEG(18°) to RAD
    GLfloat rightX = rotateY * (M_PI / 180 * 18);
    GLfloat rightY = 0;

    const GLfloat shapeVertices[] = { centerX / m_width, centerY / m_height, leftX / m_width, leftY / m_height,
        rotateX / m_width, rotateY / m_height, rightX / m_width, rightY / m_height };

    if (!ExecuteDrawStar(position, DRAW_COLOR, shapeVertices, sizeof(shapeVertices))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw shape failed");
        return;
    }

    // Convert DEG(72°) to RAD
    GLfloat rad = M_PI / 180 * 72;
    // Rotate four times
    for (int i = 0; i < NUM_4; ++i) {
        Rotate2d(centerX, centerY, &rotateX, &rotateY, rad);
        Rotate2d(centerX, centerY, &leftX, &leftY, rad);
        Rotate2d(centerX, centerY, &rightX, &rightY, rad);

        const GLfloat shapeVertices[] = { centerX / m_width, centerY / m_height, leftX / m_width, leftY / m_height,
            rotateX / m_width, rotateY / m_height, rightX / m_width, rightY / m_height };

        if (!ExecuteDrawStar(position, DRAW_COLOR, shapeVertices, sizeof(shapeVertices))) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw shape failed");
            return;
        }
    }

    if (!FinishDraw()) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw FinishDraw failed");
        return;
    }
    hasDraw = 1;

    m_flag = true;
}

void EGLCore::ChangeColor(int& hasChangeColor)
{
    if (!m_flag) {
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore", "ChangeColor");
    GLint position = PrepareDraw();
    if (position == POSITION_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "ChangeColor get position failed");
        return;
    }

    if (!ExecuteDraw(position, BACKGROUND_COLOR,
                     BACKGROUND_RECTANGLE_VERTICES, sizeof(BACKGROUND_RECTANGLE_VERTICES))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "ChangeColor execute draw background failed");
        return;
    }

    // Divided into five quadrilaterals and calculate one of the quadrilateral's Vertices
    GLfloat rotateX = 0;
    GLfloat rotateY = FIFTY_PERCENT * m_height;
    GLfloat centerX = 0;
    // Convert DEG(54° & 18°) to RAD
    GLfloat centerY = -rotateY * (M_PI / 180 * 54) * (M_PI / 180 * 18);
    // Convert DEG(18°) to RAD
    GLfloat leftX = -rotateY * (M_PI / 180 * 18);
    GLfloat leftY = 0;
    // Convert DEG(18°) to RAD
    GLfloat rightX = rotateY * (M_PI / 180 * 18);
    GLfloat rightY = 0;

    const GLfloat shapeVertices[] = { centerX / m_width, centerY / m_height, leftX / m_width, leftY / m_height,
        rotateX / m_width, rotateY / m_height, rightX / m_width, rightY / m_height };

    if (!ExecuteDrawNewStar(0, CHANGE_COLOR, shapeVertices, sizeof(shapeVertices))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw shape failed");
        return;
    }

    // Convert DEG(72°) to RAD
    GLfloat rad = M_PI / 180 * 72;
    // Rotate four times
    for (int i = 0; i < NUM_4; ++i) {
        Rotate2d(centerX, centerY, &rotateX, &rotateY, rad);
        Rotate2d(centerX, centerY, &leftX, &leftY, rad);
        Rotate2d(centerX, centerY, &rightX, &rightY, rad);
        const GLfloat shapeVertices[] = { centerX / m_width, centerY / m_height, leftX / m_width, leftY / m_height,
            rotateX / m_width, rotateY / m_height, rightX / m_width, rightY / m_height };

        if (!ExecuteDrawNewStar(position, CHANGE_COLOR, shapeVertices, sizeof(shapeVertices))) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw shape failed");
            return;
        }
    }

    if (!FinishDraw()) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "ChangeColor FinishDraw failed");
    }
    hasChangeColor = 1;
}

GLint EGLCore::PrepareDraw()
{
    if ((m_eglDisplay == nullptr) || (m_eglSurface == nullptr) || (m_eglContext == nullptr) ||
        (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "PrepareDraw: param error");
        return POSITION_ERROR;
    }

    // The gl function has no return value.
    glViewport(DEFAULT_X_POSITION, DEFAULT_Y_POSITION, m_width, m_height);
    glClearColor(GL_RED_DEFAULT, GL_GREEN_DEFAULT, GL_BLUE_DEFAULT, GL_ALPHA_DEFAULT);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_program);
    return glGetAttribLocation(m_program, POSITION_NAME);
}

bool EGLCore::ExecuteDraw(GLint position, const GLfloat* color, const GLfloat shapeVertices[], unsigned long vertSize)
{
    if ((position > 0) || (color == nullptr) || (vertSize / sizeof(shapeVertices[0])) != SHAPE_VERTICES_SIZE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "ExecuteDraw: param error");
        return false;
    }

    // The gl function has no return value.
    glVertexAttribPointer(position, POINTER_SIZE, GL_FLOAT, GL_FALSE, 0, shapeVertices);
    glEnableVertexAttribArray(position);
    glVertexAttrib4fv(1, color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, TRIANGLE_FAN_SIZE);
    glDisableVertexAttribArray(position);

    return true;
}

bool EGLCore::ExecuteDrawStar(
    GLint position, const GLfloat* color, const GLfloat shapeVertices[], unsigned long vertSize)
{
    if ((position > 0) || (color == nullptr) || (vertSize / sizeof(shapeVertices[0])) != SHAPE_VERTICES_SIZE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "ExecuteDraw: param error");
        return false;
    }

    // The gl function has no return value.
    glVertexAttribPointer(position, POINTER_SIZE, GL_FLOAT, GL_FALSE, 0, shapeVertices);
    glVertexAttribPointer(1, POINTER_SIZE, GL_FLOAT, GL_FALSE, 0, color);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(1);
    glVertexAttrib4fv(1, color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, TRIANGLE_FAN_SIZE);
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(1);

    return true;
}

bool EGLCore::ExecuteDrawNewStar(
    GLint position, const GLfloat* color, const GLfloat shapeVertices[], unsigned long vertSize)
{
    if ((position > 0) || (color == nullptr) || (vertSize / sizeof(shapeVertices[0])) != SHAPE_VERTICES_SIZE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "ExecuteDraw: param error");
        return false;
    }

    // The gl function has no return value.
    glVertexAttribPointer(position, POINTER_SIZE, GL_FLOAT, GL_FALSE, 0, shapeVertices);
    glEnableVertexAttribArray(position);
    glVertexAttrib4fv(1, color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, TRIANGLE_FAN_SIZE);
    glDisableVertexAttribArray(position);

    return true;
}

void EGLCore::Rotate2d(GLfloat centerX, GLfloat centerY, GLfloat* rotateX, GLfloat* rotateY, GLfloat theta)
{
    GLfloat tempX = cos(theta) * (*rotateX - centerX) - sin(theta) * (*rotateY - centerY);
    GLfloat tempY = sin(theta) * (*rotateX - centerX) + cos(theta) * (*rotateY - centerY);
    *rotateX = tempX + centerX;
    *rotateY = tempY + centerY;
}

bool EGLCore::FinishDraw()
{
    // The gl function has no return value.
    glFlush();
    glFinish();
    return eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

GLuint EGLCore::LoadShader(GLenum type, const char* shaderSrc)
{
    if ((type <= 0) || (shaderSrc == nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "glCreateShader type or shaderSrc error");
        return PROGRAM_ERROR;
    }

    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "glCreateShader unable to load shader");
        return PROGRAM_ERROR;
    }

    // The gl function has no return value.
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != 0) {
        return shader;
    }

    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen <= 1) {
        glDeleteShader(shader);
        return PROGRAM_ERROR;
    }
    std::string newInfoLog;
    newInfoLog.reserve(infoLen + 1);
    if (newInfoLog != "") {
        std::fill_n(newInfoLog.data(), 0, infoLen + 1);
        glGetShaderInfoLog(shader, infoLen, nullptr, newInfoLog.data());
        free(newInfoLog.data());
        newInfoLog = nullptr;
    }
    glDeleteShader(shader);
    return PROGRAM_ERROR;
}

GLuint EGLCore::CreateProgram(const char* vertexShader, const char* fragShader)
{
    if ((vertexShader == nullptr) || (fragShader == nullptr)) {
        OH_LOG_Print(
            LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "createProgram: vertexShader or fragShader is null");
        return PROGRAM_ERROR;
    }

    GLuint vertex = LoadShader(GL_VERTEX_SHADER, vertexShader);
    if (vertex == PROGRAM_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "createProgram vertex error");
        return PROGRAM_ERROR;
    }

    GLuint fragment = LoadShader(GL_FRAGMENT_SHADER, fragShader);
    if (fragment == PROGRAM_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "createProgram fragment error");
        return PROGRAM_ERROR;
    }

    GLuint program = glCreateProgram();
    if (program == PROGRAM_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "createProgram program error");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return PROGRAM_ERROR;
    }

    // The gl function has no return value.
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != 0) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return program;
    }

    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "createProgram linked error");
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 1) {
        std::string newInfoLog;
        newInfoLog.reserve(infoLen + 1);
        std::fill_n(newInfoLog.data(), 0, infoLen + 1);
        glGetProgramInfoLog(program, infoLen, nullptr, newInfoLog.data());
        free(newInfoLog.data());
        newInfoLog = nullptr;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteProgram(program);
    return PROGRAM_ERROR;
}

void EGLCore::UpdateSize(int width, int height)
{
    m_width = width;
    m_height = height;
//    if (width > 0) {
//        m_widthPercent = FIFTY_PERCENT * height / width;
//    }
}

void EGLCore::Release()
{
    m_render->ClearFrame();
    FinishLoad();
    m_render->ReleaseTexture();
    if ((nullptr == m_eglDisplay) || (nullptr == m_eglSurface) || (!eglDestroySurface(m_eglDisplay, m_eglSurface))) {
        LOGE("Release eglDestroySurface failed");
    }

    if ((nullptr == m_eglDisplay) || (nullptr == m_eglContext) || (!eglDestroyContext(m_eglDisplay, m_eglContext))) {
        LOGE("Release eglDestroyContext failed");
    }
    
    if ((nullptr == m_eglDisplay) || (!eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))) {
        LOGE("eglMakeCurrent failed");
    }

    if ((nullptr == m_eglDisplay) || (!eglTerminate(m_eglDisplay))) {
        LOGE("Release eglTerminate failed");
    }
}

void EGLCore::Render(RenderData &renderData)
{
    if (renderData.data.empty()) {
        LOGE("Yuv data is empty, unable to render YUV");
        return;
    }
    if (!SeparateYUV(renderData.data, renderData.stride, renderData.sliceHeight, m_animConfig->videoWidth,
        m_animConfig->videoHeight)) {
        LOGE("unable to readYuvFile");
    }
    glViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
    m_render->RenderFrame();
    m_frameCurIdx = renderData.frameCurIdx;
    if (m_mixRender && m_mixRender->haveSrc) {
        MixRenderFrame(renderData.frameCurIdx);
    }
    FinishLoad();
}

template <typename T>
void addVecData(std::vector<unsigned char> &data, T &&value)
{
    data.emplace_back(std::forward<T>(value));
}

bool EGLCore::SeparateYUV(const std::vector<uint8_t> &dataVec,
    int alignWidth, int alignHeight, int videoWidth, int videoHeight)
{
    std::vector<unsigned char> data_y;
    std::vector<unsigned char> data_u;
    std::vector<unsigned char> data_v;
    data_y.reserve(videoWidth * videoHeight);
    data_u.reserve(videoWidth * videoHeight / FOUR);
    data_v.reserve(videoWidth * videoHeight / FOUR);
    
    const uint8_t *data = dataVec.data();

    for (int j = 0; j < alignHeight; j++) {
        if (alignHeight != videoHeight && j >= videoHeight)
            break;
        std::vector<unsigned char> oneLine;
        oneLine.assign(data + alignWidth * j, data + alignWidth * j + videoWidth);
        data_y.insert(data_y.end(), oneLine.begin(), oneLine.end());
    }
    for (int j = 0; j < videoHeight / TWO; j++) {
        std::vector<uint8_t> oneLine;
        oneLine.assign(data + alignWidth * alignHeight + alignWidth * j,
            data + alignWidth * alignHeight + alignWidth * j + videoWidth);
        if ((oneLine.size() % TWO) != 0) {
            LOGE("error uv data");
            break;
        }
        for (int k = 0; k < oneLine.size(); k += TWO) {
            addVecData(data_u, std::move(oneLine[k]));
            addVecData(data_v, std::move(oneLine[k + 1]));
        }
    }
    m_render->SetYUVData(m_animConfig->videoWidth,
        m_animConfig->videoHeight, data_y, data_u, data_v);
    return true;
}

bool EGLCore::FinishLoad()
{
    glFlush();
    glFinish();
    // 窗口显示，交换双缓冲区
    bool swap = eglSwapBuffers(m_eglDisplay, m_eglSurface);
    m_flag = true;
    return swap;
}

static bool calClick(int32_t x, int32_t y, PointRect frame)
{
    return x >= frame.x && x <= (frame.x + frame.w)
        && y >= frame.y && y <= (frame.y + frame.h);
}

void EGLCore::GetCurrentClickTxt(int32_t eventX, int32_t eventY, std::string &clickRes)
{
    if (!m_mixRender || !m_mixRender->haveSrc) {
        LOGE("no src,  not have mix render");
        return;
    }
    int32_t viewWith = m_width;
    int32_t viewHeight = m_height;
    int32_t videoWith = m_animConfig->width;
    int32_t videoHeight = m_animConfig->height;

    if (viewWith == 0 || viewHeight == 0)
        return;
    
    int32_t x = eventX * videoWith / viewWith;
    int32_t y = eventY * videoHeight / viewHeight;
    
    auto frameIt = m_animConfig->frameAllPtr->frameAll.find(m_frameCurIdx);
    if (frameIt == m_animConfig->frameAllPtr->frameAll.end()) {
        LOGE("not find frame");
        return;
    }
    size_t perFrameSize = frameIt->second.frames.size();
    for (int i = 0; i < perFrameSize; i++) {
        Frame frame = frameIt->second.frames.at(i);
        std::string srcId = frame.srcId;
        auto it = m_animConfig->srcMapPtr->srcSMap.find(srcId);
        if (it != m_animConfig->srcMapPtr->srcSMap.end()) {
            // 找到了指定key的元素
            bool cal = calClick(x, y, frame.frame);
            Src src = it->second;
            if (cal) {
                clickRes = src.txt;
                break;
            }
            LOGD("cal click :%{public}d * %{public}d  cal :%{public}d txt: %{public}s", x, y, cal, src.txt.c_str());
        }
    }
}

EGLConfig EGLCore::ChooseConfig()
{
    std::vector<int> attribList;
    GetAttributes(attribList);
    EGLConfig configs = nullptr;
    int configsNum;

    if (!eglChooseConfig(m_eglDisplay, attribList.data(), &configs, 1, &configsNum)) {
        LOGE("eglChooseConfig: unable to choose configs");
        return nullptr;
    }
    return configs;
}

void EGLCore::GetAttributes(std::vector<int> &array)
{
    array = {
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        // 指定渲染api类别
        EGL_RED_SIZE,
        8,
        EGL_GREEN_SIZE,
        8, EGL_BLUE_SIZE,
        8,
        EGL_ALPHA_SIZE,
        8,
        EGL_DEPTH_SIZE,
        0,
        EGL_STENCIL_SIZE,
        0,
        EGL_NONE
    };
}

EGLContext EGLCore::CreateContext(EGLDisplay eglDisplay, EGLConfig eglConfig)
{
    std::vector<EGLint> attrs = {EGL_CONTEXT_CLIENT_VERSION, TWO, EGL_NONE};
    EGLContext eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, attrs.data());
    return eglContext;
}

void EGLCore::MixRenderFrame(int32_t frameCurIdx)
{
    if (!m_mixRender || !m_mixRender->haveSrc) {
        LOGE("no src, not have mix render");
        return;
    }
    MixConfigSize config = {
        m_animConfig->width,
        m_animConfig->height,
        m_animConfig->videoWidth,
        m_animConfig->videoHeight
    };
    
    auto frameIt = m_animConfig->frameAllPtr->frameAll.find(frameCurIdx);
    if (frameIt == m_animConfig->frameAllPtr->frameAll.end()) {
        LOGE("not find frame");
        return;
    }
    size_t perFrameSize = frameIt->second.frames.size();
    for (int i = 0; i < perFrameSize; i++) {
        Frame frame = frameIt->second.frames.at(i);
        std::string srcId = frame.srcId;
        auto it = m_animConfig->srcMapPtr->srcSMap.find(srcId);
        if (it != m_animConfig->srcMapPtr->srcSMap.end()) {
            // 找到了指定key的元素
            Src src = it->second;
            m_mixRender->SetVideoTextureId(m_render->GetExternalTexture());
            m_mixRender->RenderFrame(config, frame, src);
        } else {
            // 没有找到指定key的元素
            LOGE("not find key :%{public}s", srcId.c_str());
        }
    }
}

void EGLCore::InitFitConfig()
{
    if (m_fitType == VideoFitType::FIT_CENTER) {
        FitCenter();
    } else if (m_fitType == VideoFitType::CENTER_CROP) {
        CenterCrop();
    } else {
        FitXY();
    }
    
    LOGD("InitFitConfig %{public}d viewport %{public}d %{public}d %{public}d %{public}d", m_fitType,
        m_viewportWidth, m_viewportHeight, m_viewportX, m_viewportY);
}

void EGLCore::FitCenter()
{
    int32_t width = 0;
    width = m_animConfig->width;
    int32_t height = 0;
    height = m_animConfig->height;

    if (height <= 0 || width <= 0|| m_height <= 0) {
        LOGD("param is zero. height: %{public}d m_height: %{public}d ", height, m_height);
        return;
    }
    // 计算图像和目标区域的宽高比
    double ratioImage = 1.0 * width / height;
    double ratioTarget = 1.0 * m_width / m_height;

    // 选择缩放因子
    double scale;
    if (ratioImage > ratioTarget) {
        scale = 1.0 * m_width / width;
    } else {
        scale = 1.0 * m_height / height;
    }

    // 计算缩放后的图像尺寸
    int32_t newWidth = width * scale;
    int32_t newHeight = height * scale;

    // 计算图像在目标区域中的位置
    int32_t leftMargin = (m_width - newWidth) / TWO;
    int32_t topMargin = (m_height - newHeight) / TWO;

    m_viewportWidth = newWidth;
    m_viewportHeight = newHeight;
    m_viewportX = leftMargin;
    m_viewportY = topMargin;
}

void EGLCore::CenterCrop()
{
    int32_t width = 0;
    width = m_animConfig->width;
    int32_t height = 0;
    height = m_animConfig->height;

    if (height <= 0 || width <= 0|| m_height <= 0) {
        LOGD("param is zero. height: %{public}d m_height: %{public}d ", height, m_height);
        return;
    }
    // 计算图像和目标区域的宽高比
    double ratioImage = 1.0 * width / height;
    double ratioTarget = 1.0 * m_width / m_height;
    
    // 计算缩放后的图像尺寸
    int32_t newWidth;
    int32_t newHeight;

    if (ratioImage < ratioTarget) {
        newWidth = m_width;
        newHeight = 1.0 * m_width / ratioImage;
    } else {
        newHeight = m_height;
        newWidth = 1.0 * ratioImage * m_height;
    }

    // 计算裁剪区域的偏移
    int32_t leftMargin = (newWidth - m_width) / TWO;
    int32_t topMargin = (newHeight - m_height) / TWO;

    m_viewportWidth = newWidth;
    m_viewportHeight = newHeight;
    m_viewportX = -leftMargin;
    m_viewportY = -topMargin;
}

void EGLCore::FitXY()
{
    m_viewportWidth = m_width;
    m_viewportHeight = m_height;
    m_viewportX = 0;
    m_viewportY = 0;
}