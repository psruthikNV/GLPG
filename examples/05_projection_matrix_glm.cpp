#include "native_window.hpp"
#include "opengl_context.hpp"
#include "utils/opengl_shader_utils.hpp"
#include "math/glpg_math.hpp"
#include "utils/misc_utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace GLPG;

const float vertexData[] = {
    -50.0f, -50.0f, -0.2f,
    50.0f, -50.0f, -0.2f,
    0.0f, 50.0f, -0.2f
};

const char *vertexSource = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n "
    "void main() {\n"
    "   //gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);\n"
    "   gl_Position = modelMatrix * viewMatrix * projectionMatrix * vec4(vertexPosition, 1.0);\n"
    "}\0";

const char *fragmentSource = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";

int main(int argc, char **argv)
{
    GLPGWindow win(800, 600);
    GLPGContext gc;
    GLuint VBO;
    GLuint VAO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint modelMatrixLocation = 0;
    GLuint cameraMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    GLuint projectionMatrixLocation = 0;

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

    glEnable(GL_DEPTH_TEST);
    glUseProgram(programObj);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
    glEnableVertexAttribArray(0);
    modelMatrixLocation = glGetUniformLocation(programObj, "modelMatrix");
    viewMatrixLocation = glGetUniformLocation(programObj, "viewMatrix");
    projectionMatrixLocation = glGetUniformLocation(programObj, "projectionMatrix");

    glm::vec3 eye = {0.0, 0.0, 1.0};
    glm::vec3 center = {0.0, 0.0, -1.0};
    glm::vec3 up = {0.0, 1.0, 0.0};
    glm::vec3 translateVector = {0.0f, 0.0f, 0.0f};

    glm::mat4x4 viewMatrix = glm::lookAt(eye, center, up);
    //glm::mat4x4 projectionMatrix = glm::frustum(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 100.0f);
    glm::mat4x4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.f, 0.1f, 100.0f);
    glm::mat4x4 modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, translateVector);

    if (0) {
        vec3_f upVector = {0.0, 1.0, 0.0};
        vec3_f eyePosition = {0.0, 0.0, 1.0};
        vec3_f viewVector = {0.0, 0.0, -1.0};
        vec3_f translateVector = {0.0, 0.0, 0.0f};
        mat4x4_f projectionMatrix = frustum(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 100.0f);
        std::cout << "Projection Matrix : " << projectionMatrix << std::endl;
        mat4x4_f modelMatrix;
        modelMatrix = translate(modelMatrix, translateVector);
        mat4x4_f viewMatrix = lookAt(eyePosition, viewVector, upVector);
    }
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
    while (1) {
	    glDrawArrays(GL_TRIANGLES, 0, 3);
	    gc.swapBuffers();
    }
    pause();
}
