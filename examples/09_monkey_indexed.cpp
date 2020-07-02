#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "utils/GLPGShaderUtils.hpp"
#include "math/GLPGMath.hpp"
#include "utils/GLPGUtils.hpp"

using namespace GLPG;

vec3_f trianglePositions[] = {
  vec3_f({0.0f,  0.0f,  0.0f}),
};
const char *vertexSource = 
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

const char *fragmentSource = 
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
    GLPGWindow win(800, 600);
    GLPGContext gc;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint modelMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    GLuint projectionMatrixLocation = 0;

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

    if (!win.createNativeWindow()) {
        std::cout << "Failed to create native window" << std::endl;
        return -1;
    }

    if (!gc.initializeGlContext(win, 4, 5)) {
        std::cout << "Failed to initialize GL Context" << std::endl;
        return -1;
    }

    vtxShaderObj = glCreateShader(GL_VERTEX_SHADER);
    fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    programObj = glCreateProgram();

    if (!compileShader(vtxShaderObj, vertexSource)) {
        std::cout << "Failed to compile GLSL Vertex Shader" << std::endl;
        return -1;
    }

    if (!compileShader(fragShaderObj, fragmentSource)) {
        std::cout << "Failed to compile GLSL Fragment Shader" << std::endl;
        return -1;
    }

    glAttachShader(programObj, vtxShaderObj);
    glAttachShader(programObj, fragShaderObj);

    if (!linkShaders(programObj)) {
        std::cout << "Failed to link GLSL Shaders" << std::endl;
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
    SYSTEMTIME sysTime;
    LARGE_INTEGER timer;
    _LARGE_INTEGER test;

    modelMatrix = translate(modelMatrix, translateVector);
    viewMatrix = lookAt(eyePosition, viewVector, upVector);
    vec3_f viewMatrix;
    glClearColor(0.0, 1.0, 1.0, 1.0);
    while (1) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < 1; i++) {
            //std::cout << "Frame : " << i << "\n";
            GetSystemTime(&sysTime);
            //std::cout << "Seconds : " << sysTime.wSecond << "\n";
            vec3_f eyePos = {(float)(sin(sysTime.wMilliseconds) * 10.0), 0.0F, (float)(cos(sysTime.wMilliseconds) * 10.0)};
            modelMatrix.identity();
            modelMatrix = translate(modelMatrix, trianglePositions[i]);
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            const float radius = 10.0f;
            QueryPerformanceCounter(&timer);

            float camX = sin(timer.QuadPart * 0.00000001F) * radius;
            float camZ = cos(timer.QuadPart * 0.00000001F) * radius;
            viewMatrix = lookAt(vec3_f({camX, 0.0, camZ}), vec3_f({0.0, 0.0, 0.0}), vec3_f({0.0, 1.0, 0.0}));
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
            //glDrawArrays(GL_TRIANGLES, 0, monkeyVertices.size());
            glDrawElements(GL_TRIANGLES, vtxIdx.size(), GL_UNSIGNED_INT, 0);
        }
	    gc.swapBuffers();
    }
}
