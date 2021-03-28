#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "GLPGEvent.hpp"
#include "utils/GLPGShaderUtils.hpp"
#include "utils/GLPGUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "3rdparty/stb_image.h"

const float vertexData[] = {
    -0.5f, -0.5f, 0.0f, 0.0F, 0.0F,
    0.5f, -0.5f, 0.0f, 1.0F, 0.0F,
    0.0f, 0.5f, 0.0f, 0.5F, 1.0F
};

const char *vertexShaderSource =
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(vertexPosition, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = 
    "#version 450 core\n"
    "out vec4 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "   fragmentColor = vec4(1.0, 0.0, 0.0, 0.0);\n"
    "}\0";

int main()
{
    GLuint VBO = 0U;
    GLuint VAO = 0U;
    GLuint vtxShaderObj = 0U;
    GLuint fragShaderObj = 0U;
    GLuint programObj = 0U;
    GLuint textureObj = 0U;
    GLPG::GLPGContext context;
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;
    const char *texturePath = "/home/sruthik/repos/GLPG/assets/textures/road-texture.jpg";
    int textureWidth = 0;
    int textureHeight = 0;
    int textureComponents = 0;

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to create GLPGWindow\n";
        return -1;
    }

    if (window->CreateWindow(640, 480)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }

    if (!context.InitializeContext()) {
        std::cerr << "Failed to create GL Context\n";
        return -1;
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
    glGenTextures(1, &textureObj);
    glBindTexture(GL_TEXTURE_2D, textureObj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, textureData);
    if (!GLPG::CheckGLError()) {
        std::cerr << "glTexImage2D failed\n";
        return -1;
    }
    glEnable(GL_STENCIL_TEST);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glFlush();
    context.SwapBuffers();
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        context.SwapBuffers();
    }
    return 0;
}
