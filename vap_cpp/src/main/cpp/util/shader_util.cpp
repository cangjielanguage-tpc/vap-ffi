/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "shader_util.h"
#include <string>
#include "log.h"

static std::unordered_map<std::string, GLuint> s_programCache;

GLuint ShaderUtil::CreateProgram(const char *vertexSource, const char *fragmentSource)
{
    GLuint vertexShaderHandle = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShaderHandle) {
        return 0;
    }
    GLuint fragmentShaderHandle = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShaderHandle) {
        glDeleteShader(vertexShaderHandle);
        return 0;
    }
    return CreateAndLinkProgram(vertexShaderHandle, fragmentShaderHandle);
}

GLuint ShaderUtil::CompileShader(GLenum shaderType, const char *shaderSource)
{
    GLuint shaderHandle = glCreateShader(shaderType);
    if (shaderHandle == 0) {
        return 0;
    }
    
    GLint compileStatus;
    glShaderSource(shaderHandle, 1, &shaderSource, NULL);
    glCompileShader(shaderHandle);
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        GLint infoLen = 0;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            std::string infoLog(infoLen, '\0');
            glGetShaderInfoLog(shaderHandle, infoLen, nullptr, (GLchar *)&infoLog);
        }

        glDeleteShader(shaderHandle);
        return 0;
    }
    return shaderHandle;
}

GLuint ShaderUtil::CreateAndLinkProgram(GLuint vertexShaderHandle, GLuint fragmentShaderHandle)
{
    GLuint programHandle = glCreateProgram();
    GLint linkStatus;
    
    if (programHandle != 0) {
        glAttachShader(programHandle, vertexShaderHandle);
        glAttachShader(programHandle, fragmentShaderHandle);
        glLinkProgram(programHandle);
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) {
            GLint infoLen = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                std::string infoLog(infoLen, '\0');
                glGetProgramInfoLog(programHandle, infoLen, nullptr, (GLchar *)&infoLog);
            }
            glDeleteProgram(programHandle);
            return 0;
        }
    }
    return programHandle;
}