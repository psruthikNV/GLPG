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
#include "utils/GLPGUtils.hpp"

using namespace GLPG;

const char *vertexShaderSource = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "flat out int glpg_vertexID;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n "
    "void main() {\n"
    "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);\n"
    "   glpg_vertexID = gl_VertexID;\n"
    "}\0";

const char *fragmentShaderSource = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "flat in int glpg_vertexID;\n"
    "void main() {\n"
    "switch (glpg_vertexID) {\n"
    "   case 0:\n"
    "       fragColor = vec4(0.0, 1.0, 0.0, 1.0); break;\n"
    "   case 1:\n"
    "       fragColor = vec4(1.0, 0.0, 0.0, 1.0);break;\n"
    "   case 2:\n"
    "       fragColor = vec4(0.0, 0.0, 1.0, 1.0);break;\n"
    "   case 3:\n"
    "       fragColor = vec4(0.5, 0.0, 0.0, 1.0);break;\n"
    "   case 4:\n"
    "       fragColor = vec4(0.0, 0.5, 0.0, 1.0);break;\n"
    "   case 5:\n"
    "       fragColor = vec4(0.0, 0.0, 0.5, 1.0);break;\n"
    "   case 6:\n"
    "       fragColor = vec4(0.5, 0.5, 0.5, 1.0);break;\n"
    "   case 7:\n"
    "       fragColor = vec4(0.0, 0.0, 0.0, 1.0);break;\n"
    "   //default:\n"
    "       //fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n"
    "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";

int main(int argc, char **argv)
{
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint modelMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    GLuint projectionMatrixLocation = 0;
    GLPGContext context;
    GLPGEventLoop eventLoop;
    GLPGEvent event;
    std::vector<VertexIN> monkeyVertices;
    std::vector<FaceIN> faceStuff;

    uint32_t indices[] = {
        1, 2, 4,
        2, 4, 3,
        3, 4, 8,
        3, 8, 7,
        7, 8, 6,
        7, 6, 5,
        5, 6, 2,
        5, 2, 1,
        3, 7, 5,
        3, 5, 1,
        8, 4, 2,
        8, 4, 6
    };

    for (uint32_t i = 0U; i < 36; i++) {
        indices[i] -= 1;
    }

    if (!LoadObjFile("C:\\Users\\psrut\\3D Objects\\monkey.obj", monkeyVertices, faceStuff)) {
    //if (!LoadObjFile("C:\\Users\\Sruthik\\3D Objects\\cube.obj", monkeyVertices, faceStuff)) {
        std::cout << "Failed to load Vertices\n";
        return -1;
    } else {
        std::cout << "Loaded Monkey Vertices\n";
    }

    std::vector<uint32_t> vtxIdx;
    std::cout << "Monkey Vertices sizee : " << monkeyVertices.size() << "\n";
    std::cout << "Face Stuff size : " << faceStuff.size() << "\n";
    std::cout << "vertexIndices size : " << faceStuff[0].vertexIndices.size() << "\n";

    for (uint32_t i = 0U; i < faceStuff.size(); i++) {
        if (faceStuff[i].vertexIndices.size() == 4) {
            vtxIdx.push_back(faceStuff[i].vertexIndices[0]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[1]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[2]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[2]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[3]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[0]);
        } else if (faceStuff[i].vertexIndices.size() == 3) {
            vtxIdx.push_back(faceStuff[i].vertexIndices[0]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[1]);
            vtxIdx.push_back(faceStuff[i].vertexIndices[2]);
        }
    }

    std::cout << "Size of vtxIdx : " << vtxIdx.size() << "\n";

    GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
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
    
    glEnable(GL_DEPTH_TEST);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexIN) * monkeyVertices.size(), monkeyVertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vtxIdx.size() * sizeof(uint32_t), vtxIdx.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *)0);
    glEnableVertexAttribArray(0);
    modelMatrixLocation = glGetUniformLocation(programObj, "modelMatrix");
    viewMatrixLocation = glGetUniformLocation(programObj, "viewMatrix");
    projectionMatrixLocation = glGetUniformLocation(programObj, "projectionMatrix");
    vec3_f eyePosition = {0.0f, 0.0f, 3.0f};
    vec3_f upVector = {0.0f, 1.0f, 0.0f};
    vec3_f viewVector = {0.0f, 0.0f, 0.0f};
    vec3_f translateVector = {0.0f, 0.0f, 0.0f};
    mat4x4_f projectionMatrix = gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    mat4x4_f modelMatrix;
    mat4x4_f viewMatrix;

    modelMatrix = translate(modelMatrix, translateVector);
    viewMatrix = lookAt(eyePosition, viewVector, upVector);
    glClearColor(0.0, 1.0, 1.0, 1.0);
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < 1; i++) {
            modelMatrix.identity();
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            const float radius = 10.0f;

            glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
            //glDrawArrays(GL_TRIANGLES, 0, monkeyVertices.size());
            glDrawElements(GL_TRIANGLES, vtxIdx.size(), GL_UNSIGNED_INT, 0);
        }
        context.SwapBuffers();
    }
}
