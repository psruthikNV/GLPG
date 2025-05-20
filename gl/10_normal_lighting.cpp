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

enum class lightingModel: uint32_t {
    Z_ONLY,
    NORMAL_ONLY,
    NORMAL_AND_Z,
    LAST = NORMAL_AND_Z
};

const float vertexData[] = {

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0F, 0.0F, -1.0F, // +Z
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0F, 0.0F, -1.0F, // +Z
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0F, 0.0F, -1.0F,// +Z
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0F, 0.0F, -1.0F,// +Z
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0F, 0.0F, -1.0F,// +Z
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0F, 0.0F, -1.0F,// +Z

    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0F, 0.0F, 0.0F,// -X
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, -1.0F, 0.0F, 0.0F,// -X
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0F, 0.0F, 0.0F,// -X
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0F, 0.0F, 0.0F,// -X
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -1.0F, 0.0F, 0.0F,// -X
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0F, 0.0F, 0.0F,// -X

     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0F, 0.0F, 0.0F,// +X
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0F, 0.0F, 0.0F,// +X
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0F, 0.0F, 0.0F,// +X
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0F, 0.0F, 0.0F,// +X
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0F, 0.0F, 0.0F,// +X
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0F, 0.0F, 0.0F,// +X

    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0F, -1.0F, 0.0F,// -Y
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0F, -1.0F, 0.0F,// -Y
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0F, -1.0F, 0.0F,// -Y
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0F, -1.0F, 0.0F,// -Y
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0F, -1.0F, 0.0F,// -Y
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0F, -1.0F, 0.0F,// -Y

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0F, 1.0F, 0.0F,// +Y
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0F, 1.0F, 0.0F,// +Y
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0F, 1.0F, 0.0F,// +Y
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0F, 1.0F, 0.0F,// +Y
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0F, 1.0F, 0.0F,// +Y
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0F, 1.0F, 0.0F,// +Y
    
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0F, 0.0F, 1.0F,// -Z
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0F, 0.0F, 1.0F,// -Z
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0F, 0.0F, 1.0F,// -Z
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0F, 0.0F, 1.0F,// -Z
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0F, 0.0F, 1.0F,// -Z
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0F, 0.0F, 1.0F// -Z
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
    "layout (location = 2) in vec3 vertexNormals;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n "
    "out vec2 texCoords;\n"
    "out float zDistance;\n"
    "out vec3 vtxNormals;\n"
    "out vec3 fragPos;\n"
    "void main() {\n"
    "   vec3 normal;\n"
    "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);\n"
    "   texCoords = textureCoords;\n"
    "   zDistance = gl_Position.z;\n"
    "   vtxNormals = vertexNormals;\n"
    "   fragPos = vec3(modelMatrix * vec4(vertexPosition, 1.0));\n"
    "}\0";

const char *fragmentShaderSource_zOnly = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoords;\n"
    "in float zDistance;\n"
    "in vec3 vtxNormals;\n"
    "uniform sampler2D sampler;\n"
    "uniform vec3 eyePosition;\n"
    "in vec3 fragPos;\n"
    "void main() {\n"
    "   fragColor = texture(sampler, texCoords);\n"
    "   fragColor = fragColor / zDistance;\n"
    "}\0";
 
const char *fragmentShaderSource_normalOnly = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoords;\n"
    "in float zDistance;\n"
    "in vec3 vtxNormals;\n"
    "uniform sampler2D sampler;\n"
    "uniform vec3 eyePosition;\n"
    "in vec3 fragPos;\n"
    "void main() {\n"
    "   fragColor = texture(sampler, texCoords);\n"
    "   float dotProd = max(dot(normalize(eyePosition), normalize(vtxNormals)), 0.0);\n"
    "   fragColor = fragColor * dotProd;\n"
    "}\0";

const char *fragmentShaderSource_normalAndZ = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoords;\n"
    "in float zDistance;\n"
    "in vec3 vtxNormals;\n"
    "uniform sampler2D sampler;\n"
    "uniform vec3 eyePosition;\n"
    "in vec3 fragPos;\n"
    "void main() {\n"
    "   fragColor = texture(sampler, texCoords);\n"
    "   float dotProd = max(dot(normalize(eyePosition), normalize(vtxNormals)), 0.0);\n"
    "   fragColor = fragColor * dotProd;\n"
    "   fragColor = fragColor / zDistance;\n"
    "}\0";


int main(int argc, char **argv)
{
    // GL resources
    GLuint VBO;
    GLuint VAO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj_zOnly = 0;
    GLuint fragShaderObj_normalOnly = 0;
    GLuint fragShaderObj_normalAndZ = 0;
    GLuint programObj_zOnly = 0;
    GLuint programObj_normalOnly = 0;
    GLuint programObj_normalAndZ = 0;
    GLuint textureObj = 0U;
    GLuint modelMatrixLocation_zOnly = 0;
    GLuint viewMatrixLocation_zOnly = 0;
    GLuint projectionMatrixLocation_zOnly = 0;
    GLuint eyePositionLocation_zOnly = 0;
    GLuint modelMatrixLocation_normalOnly = 0;
    GLuint viewMatrixLocation_normalOnly = 0;
    GLuint projectionMatrixLocation_normalOnly = 0;
    GLuint eyePositionLocation_normalOnly = 0;
    GLuint modelMatrixLocation_normalAndZ = 0;
    GLuint viewMatrixLocation_normalAndZ = 0;
    GLuint projectionMatrixLocation_normalAndZ = 0;
    GLuint eyePositionLocation_normalAndZ = 0;

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
    fragShaderObj_zOnly = glCreateShader(GL_FRAGMENT_SHADER);
    fragShaderObj_normalOnly = glCreateShader(GL_FRAGMENT_SHADER);
    fragShaderObj_normalAndZ = glCreateShader(GL_FRAGMENT_SHADER);
    programObj_zOnly = glCreateProgram();
    programObj_normalOnly = glCreateProgram();
    programObj_normalAndZ = glCreateProgram();

    if (!GLPG::compileShader(vtxShaderObj, vertexShaderSource)) {
        std::cout << "Vertex Shader Compilation Failed" << std::endl;
        return -1;
    }
    if (!GLPG::compileShader(fragShaderObj_zOnly, fragmentShaderSource_zOnly)) {
        std::cout << "Fragment Shader Z only Compilation Failed" << std::endl;
        return -1;
    }
    if (!GLPG::compileShader(fragShaderObj_normalOnly, fragmentShaderSource_normalOnly)) {
         std::cout << "Fragment Shader normal only Compilation Failed" << std::endl;
         return -1;
    }
    if (!GLPG::compileShader(fragShaderObj_normalAndZ, fragmentShaderSource_normalAndZ)) {
        std::cout << "Fragment Shader normal and Z Compilation Failed" << std::endl;
        return -1;
    }

    glAttachShader(programObj_zOnly, vtxShaderObj);
    glAttachShader(programObj_zOnly, fragShaderObj_zOnly);

    if (!GLPG::linkShaders(programObj_zOnly)) {
        std::cout << "Failed to link Shaders" << std::endl;
        return -1;
    }

    glAttachShader(programObj_normalOnly, vtxShaderObj);
    glAttachShader(programObj_normalOnly, fragShaderObj_normalOnly);

    if (!GLPG::linkShaders(programObj_normalOnly)) {
        std::cout << "Failed to link Shaders" << std::endl;
        return -1;
    }

    glAttachShader(programObj_normalAndZ, vtxShaderObj);
    glAttachShader(programObj_normalAndZ, fragShaderObj_normalAndZ);

    if (!GLPG::linkShaders(programObj_normalAndZ)) {
        std::cout << "Failed to link Shaders" << std::endl;
        return -1;
    }

    glUseProgram(programObj_zOnly);
    modelMatrixLocation_zOnly = glGetUniformLocation(programObj_zOnly, "modelMatrix");
    viewMatrixLocation_zOnly = glGetUniformLocation(programObj_zOnly, "viewMatrix");
    projectionMatrixLocation_zOnly = glGetUniformLocation(programObj_zOnly, "projectionMatrix");
    eyePositionLocation_zOnly =  glGetUniformLocation(programObj_zOnly, "eyePosition");
    
    glUseProgram(programObj_normalOnly);
    modelMatrixLocation_normalOnly = glGetUniformLocation(programObj_normalOnly, "modelMatrix");
    viewMatrixLocation_normalOnly = glGetUniformLocation(programObj_normalOnly, "viewMatrix");
    projectionMatrixLocation_normalOnly = glGetUniformLocation(programObj_normalOnly, "projectionMatrix");
    eyePositionLocation_normalOnly =  glGetUniformLocation(programObj_normalOnly, "eyePosition");
    
    glUseProgram(programObj_normalAndZ);
    modelMatrixLocation_normalAndZ = glGetUniformLocation(programObj_normalAndZ, "modelMatrix");
    viewMatrixLocation_normalAndZ = glGetUniformLocation(programObj_normalAndZ, "viewMatrix");
    projectionMatrixLocation_normalAndZ = glGetUniformLocation(programObj_normalAndZ, "projectionMatrix");
    eyePositionLocation_normalAndZ =  glGetUniformLocation(programObj_normalAndZ, "eyePosition");

    glUseProgram(programObj_zOnly);
    lightingModel lightingModelInUse = lightingModel::Z_ONLY;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    vec3_f eyePosition = {0.0f, 0.0f, 3.0f};
    vec3_f upVector = {0.0f, 1.0f, 0.0f};
    vec3_f viewVector = {0.0f, 0.0f, 1.0f};
    vec3_f translateVector = {0.0f, 0.0f, 0.0f};
    float fov = 45.0F;
    mat4x4_f projectionMatrix = gluPerspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    mat4x4_f modelMatrix;
    mat4x4_f viewMatrix;

    modelMatrix = translate(modelMatrix, translateVector);
    viewMatrix = lookAtRH(eyePosition, viewVector, upVector);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //glClearColor(0.0, 1.0, 1.0, 1.0);
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        switch(event) {
            case GLPG::GLPGEvent::MouseWheel_Up:
                fov += 1.0F;
                break;
            case GLPG::GLPGEvent::MouseWheel_Down:
                fov -= 1.0F;
                break;
            case GLPG::GLPGEvent::Key_A:
                eyePosition[0] -= 0.2F;
                break;
            case GLPG::GLPGEvent::Key_D:
                eyePosition[0] += 0.2F;
                break;
            case GLPG::GLPGEvent::Key_W:
                eyePosition[1] += 0.2F;
                break;
            case GLPG::GLPGEvent::Key_S:
                eyePosition[1] -= 0.2F;
                break;
            case GLPG::GLPGEvent::Key_1:
                lightingModelInUse = lightingModel::Z_ONLY;
                break;
            case GLPG::GLPGEvent::Key_2:
                lightingModelInUse = lightingModel::NORMAL_ONLY;
                break;
            case GLPG::GLPGEvent::Key_3:
                lightingModelInUse = lightingModel::NORMAL_AND_Z;
                break;
        }

        if (eyePosition[2] < 0.2F) {
            eyePosition[2] = 0.2F;
        }
        if (fov < 40) {
            fov = 40;
        }

        if (fov > 120) {
            fov = 120;
        }

        GLuint programObjToUse = 0U;
        GLuint modelMatrixLocation = 0U;
        GLuint viewMatrixLocation = 0U;
        GLuint projectionMatrixLocation = 0U;
        GLuint eyePositionLocation = 0U;
        switch(lightingModelInUse) {
            case lightingModel::Z_ONLY:
                programObjToUse = programObj_zOnly;
                modelMatrixLocation = modelMatrixLocation_zOnly;
                viewMatrixLocation = viewMatrixLocation_zOnly;
                projectionMatrixLocation = projectionMatrixLocation_zOnly;
                eyePositionLocation = eyePositionLocation_zOnly;
                break;
            case lightingModel::NORMAL_ONLY:
                 programObjToUse = programObj_normalOnly;
                 modelMatrixLocation = modelMatrixLocation_normalOnly;
                 viewMatrixLocation = viewMatrixLocation_normalOnly;
                 projectionMatrixLocation = projectionMatrixLocation_normalOnly;
                 eyePositionLocation = eyePositionLocation_normalOnly;
                 break;
            case lightingModel::NORMAL_AND_Z:
                 programObjToUse = programObj_normalAndZ;
                 modelMatrixLocation = modelMatrixLocation_normalAndZ;
                 viewMatrixLocation = viewMatrixLocation_normalAndZ;
                 projectionMatrixLocation = projectionMatrixLocation_normalAndZ;
                 eyePositionLocation = eyePositionLocation_normalAndZ;
                 break;
        }

        glUseProgram(programObjToUse);

        projectionMatrix = gluPerspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
        viewMatrix = lookAtRH(eyePosition, viewVector, upVector);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < array_size(trianglePositions); i++) {
            modelMatrix.identity();
            modelMatrix = translate(modelMatrix, trianglePositions[i]);
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
            glUniform3f(eyePositionLocation, eyePosition[0], eyePosition[1], eyePosition[2]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ts = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        //std::cout << "Draw time: " << ts.count() * 1000 << "\n";
        auto f1 = std::chrono::high_resolution_clock::now();
	    context.SwapBuffers();
        auto f2 = std::chrono::high_resolution_clock::now();
        auto fs = std::chrono::duration_cast<std::chrono::duration<double>>(f2 - f1);
        //std::cout << "Flip time: " << fs.count() * 1000 << "\n";
    }
}
