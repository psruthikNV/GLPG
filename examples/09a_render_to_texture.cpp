#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "GLPGEvent.hpp"
#include "utils/GLPGShaderUtils.hpp"
#include "math/GLPGMath.hpp"
#include "utils/vk-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "3rdparty/stb_image.h"

using namespace GLPG;

vec3_f trianglePositions[] = {
  vec3_f({0.0f,  0.0f,  0.0f}), 
  vec3_f({2.0f,  5.0f, -15.0f}), 
  vec3_f({-1.5f, -2.2f, -2.5f}),  
  vec3_f({-3.8f, -2.0f, -12.3f}),  
  vec3_f({2.4f, -0.4f, -3.5f}),  
  vec3_f({-1.7f,  3.0f, -7.5f}),  
  vec3_f({1.3f, -2.0f, -2.5f}),  
  vec3_f({1.5f,  2.0f, -2.5f}), 
  vec3_f({1.5f,  0.2f, -1.5f}),
  vec3_f({-1.3f,  1.0f, -1.5f}),
};


const float vertexData[] = {

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f
};
/*
const float vertexData[] = {
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
*/

const char *vertexShaderSource_depthPass = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n "
    "void main() {\n"
    "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);\n"
    "}\0";

const char *fragmentShaderSource_depthPass = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";

const char *vertexShaderSource_colorPass = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "layout (location = 1) in vec2 textureCoords;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n "
    "out vec2 texCoords;\n"
    "void main() {\n"
    "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);\n"
    "   texCoords = textureCoords;\n"
    "   //gl_Position = vec4(vertexPosition, 1.0);\n"
    "}\0";

const char *fragmentShaderSource_colorPass = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoords;\n"
    "layout(binding=1) uniform sampler2D sampler;\n"
    "layout(binding=0) uniform sampler2D depthSampler;\n"
    "void main() {\n"
    "   vec2 uv = vec2(gl_FragCoord.x / 1024.0, gl_FragCoord.y / 768.0);\n"
    "   //fragColor = texture(sampler, texCoords);\n"
    "   //fragColor = texture(depthSampler, texCoords);\n"
    "   //fragColor = texture(depthSampler, uv);\n"
    "   fragColor = vec4(texture(depthSampler, uv).r, texture(depthSampler, uv).r, texture(depthSampler, uv).r, 1.0);\n"
    "   //fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";

int main(int argc, char **argv)
{
    // GL resources
    GLuint VBO;
    GLuint VAO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint depthVtxShaderObj = 0U;
    GLuint depthFragShaderObj = 0U;
    GLuint programObj = 0;
    GLuint depthProgramObj = 0;
    GLuint textureObj = 0U;
    GLuint modelMatrixLocation = 0;
    GLuint cameraMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    GLuint projectionMatrixLocation = 0;
    GLuint frameBufferObj = 0U;
    GLPGContext context;
    GLPGEventLoop eventLoop;
    GLPGEvent event;
    //const char *texturePath = "/home/sruthik/repos/GLPG/assets/textures/road-texture.jpg";
    const char *texturePath = "C:\\repos\\GLPG\\assets\\textures\\road-texture.jpg";
    //const char *texturePath = "/home/sruthik/repos/GLPG/assets/textures/wall.jpg";
    //const char *texturePath = "/home/sruthik/repos/GLPG/assets/textures/container.jpg";
    int textureWidth = 0;
    int textureHeight = 0;
    int textureComponents = 0;
    uint32_t width = 1024U;
    uint32_t height = 768U;

    GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    if (window->CreateWindow(width, height)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }
    if (!context.InitializeContext()) {
        std::cerr << "Failed to create GL Context\n";
        return -1;
    } else {
        glEnable(GL_DEPTH_TEST);
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }
    unsigned char *textureData = stbi_load(texturePath, &textureWidth, &textureHeight, &textureComponents, 0);
    if (!textureData) {
        std::cerr << "Failed to load texture data\n";
        return -1;
    }

    vtxShaderObj = glCreateShader(GL_VERTEX_SHADER);
    fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    depthVtxShaderObj = glCreateShader(GL_VERTEX_SHADER);
    depthFragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    programObj = glCreateProgram();
    depthProgramObj = glCreateProgram();

    if (!GLPG::compileShader(vtxShaderObj, vertexShaderSource_colorPass)) {
        std::cout << "Vertex Shader Compilation Failed" << std::endl;
        return -1;
    }
    if (!GLPG::compileShader(fragShaderObj, fragmentShaderSource_colorPass)) {
        std::cout << "Fragment Shader Compilation Failed" << std::endl;
        return -1;
    }

    if (!GLPG::compileShader(depthVtxShaderObj, vertexShaderSource_depthPass)) {
        std::cout << "Vertex Shader Compilation Failed" << std::endl;
        return -1;
    }
    if (!GLPG::compileShader(depthFragShaderObj, fragmentShaderSource_depthPass)) {
        std::cout << "Fragment Shader Compilation Failed" << std::endl;
        return -1;
    }


    glAttachShader(programObj, vtxShaderObj);
    glAttachShader(programObj, fragShaderObj);
    glAttachShader(depthProgramObj, depthVtxShaderObj);
    glAttachShader(depthProgramObj, depthFragShaderObj);

    if (!GLPG::linkShaders(programObj)) {
        std::cout << "Failed to link Shaders" << std::endl;
        return -1;
    }

    if (!GLPG::linkShaders(depthProgramObj)) {
        std::cout << "Failed to link depth Shaders" << std::endl;
        return -1;
    }
 
    //glUseProgram(programObj);
    glGenBuffers(1, &VBO);
    glGenFramebuffers(1, &frameBufferObj);
    
    glGenTextures(1, &textureObj);
    // Setup depth only FBO
    glBindTexture(GL_TEXTURE_2D, textureObj);
    glActiveTexture(GL_TEXTURE0); //TODO: For some reason the FBO is not complete if GL_TEXTURE0 is not the active unit
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    if (!GLPG::CheckGLError()) {
        std::cerr << "glTexImage2D failed for depth texture\n";
        return -1;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObj);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureObj, 0);
    if (!GLPG::CheckGLError()) {
        std::cerr << "glFrameBufferTexture failed\n";
    }

    glDrawBuffer(GL_NONE); // Tell GL we're not using any color buffers for drawing.
    if (!GLPG::CheckGLError()) {
        std::cerr << "glDrawBuffer failed\n";
    }

    if (auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER); ret != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Depth framebuffer not complete. Err: " << std::hex << ret << std::dec << "\n";
        return -1;
    }

    // Setup the model textures in GL_TEXTURE1 unit
    glActiveTexture(GL_TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    if (!GLPG::CheckGLError()) {
        std::cerr << "glTexImage2D failed for model texture data\n";
        return -1;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    modelMatrixLocation = glGetUniformLocation(programObj, "modelMatrix");
    viewMatrixLocation = glGetUniformLocation(programObj, "viewMatrix");
    projectionMatrixLocation = glGetUniformLocation(programObj, "projectionMatrix");
    vec3_f eyePosition = {0.0f, 0.0f, 3.0f};
    vec3_f upVector = {0.0f, 1.0f, 0.0f};
    vec3_f viewVector = {0.0f, 0.0f, 1.0f};
    vec3_f translateVector = {0.0f, 0.0f, 0.0f};
    mat4x4_f projectionMatrix = gluPerspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    mat4x4_f modelMatrix;
    mat4x4_f viewMatrix;

    modelMatrix = translate(modelMatrix, translateVector);
    viewMatrix = lookAtRH(eyePosition, viewVector, upVector);
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape){
        if (event == GLPG::GLPGEvent::MouseWheel_Up) {
            eyePosition[2] += 1.1F;
        } else if (event == GLPG::GLPGEvent::MouseWheel_Down) {
            eyePosition[2] -= 1.1F;
        }
        if (eyePosition[2] < 1.1F) {
            eyePosition[2] = 1.1F;
        }
        viewMatrix = lookAtRH(eyePosition, viewVector, upVector);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // Depth pass
        glUseProgram(depthProgramObj);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObj);
        glClearDepth(0.5F);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < array_size(trianglePositions); i++) {
            modelMatrix.identity();
            modelMatrix = translate(modelMatrix, trianglePositions[i]);
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glClearDepth(1.0F);
        // Color pass
        glUseProgram(programObj);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < array_size(trianglePositions); i++) {
            modelMatrix.identity();
            modelMatrix = translate(modelMatrix, trianglePositions[i]);
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
	    context.SwapBuffers();
    }
}
