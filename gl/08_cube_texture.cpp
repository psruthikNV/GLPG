#ifdef _WIN32
#include <Windows.h>
#endif

#include <chrono>
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
const char *vertexShaderSource = 
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

const char *fragmentShaderSource = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D sampler;\n"
    "void main() {\n"
    "   fragColor = texture(sampler, texCoords);\n"
    "}\0";

int main(int argc, char **argv)
{
    // GL resources
    GLuint VBO;
    GLuint VAO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint textureObj = 0U;
    GLuint modelMatrixLocation = 0;
    GLuint cameraMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    GLuint projectionMatrixLocation = 0;
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
    uint32_t width = 2560U;
    uint32_t height = 1440U;

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
    programObj = glCreateProgram();

    if (!GLPG::compileShader(vtxShaderObj, vertexShaderSource)) {
        std::cout << "Vertex Shader Compilation Failed" << std::endl;
        return -1;
    }
    if (!GLPG::compileShader(fragShaderObj, fragmentShaderSource)) {
        std::cout << "Fragment Shader Compilation Failed" << std::endl;
        return -1;
    }

    glAttachShader(programObj, vtxShaderObj);
    glAttachShader(programObj, fragShaderObj);

    if (!GLPG::linkShaders(programObj)) {
        std::cout << "Failed to link Shaders" << std::endl;
        return -1;
    }
 
    glUseProgram(programObj);
    glGenBuffers(1, &VBO);
    glGenTextures(1, &textureObj);
    glBindTexture(GL_TEXTURE_2D, textureObj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    if (!GLPG::CheckGLError()) {
        std::cerr << "glTexImage2D failed\n";
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
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        if (event == GLPG::GLPGEvent::MouseWheel_Up) {
            eyePosition[2] += 1.1F;
        } else if (event == GLPG::GLPGEvent::MouseWheel_Down) {
            eyePosition[2] -= 1.1F;
        }
        if (eyePosition[2] < 1.1F) {
            eyePosition[2] = 1.1F;
        }
        viewMatrix = lookAtRH(eyePosition, viewVector, upVector);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < array_size(trianglePositions); i++) {
            modelMatrix.identity();
            modelMatrix = translate(modelMatrix, trianglePositions[i]);
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ts = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Draw time: " << ts.count() * 1000 << "\n";
        auto f1 = std::chrono::high_resolution_clock::now();
	    context.SwapBuffers();
        auto f2 = std::chrono::high_resolution_clock::now();
        auto fs = std::chrono::duration_cast<std::chrono::duration<double>>(f2 - f1);
        std::cout << "Flip time: " << fs.count() * 1000 << "\n";
    }
}
