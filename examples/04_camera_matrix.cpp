#include <random>

#include "utils/native_window.hpp"
#include "utils/opengl_context.hpp"
#include "utils/opengl_shader_utils.hpp"
#include "utils/math/template_math_ops.hpp"

const float vertexData[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

const char *vertexSource = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "void main() {\n"
    "   gl_Position = modelMatrix * viewMatrix * vec4(vertexPosition, 1.0);\n"
    "}\0";

const char *fragmentSource = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";

int main(int argc, char **argv)
{
    nativeWindow win(800, 600);
    glContext gc;
    GLuint VBO;
    GLuint VAO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint modelMatrixLocation = 0;
    GLuint cameraMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;

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
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
    glEnableVertexAttribArray(0);
    modelMatrixLocation = glGetUniformLocation(programObj, "modelMatrix");
    viewMatrixLocation = glGetUniformLocation(programObj, "viewMatrix");
    std::cout << "Model Matrix Location : " << modelMatrixLocation << std::endl;
    std::cout << "View Matrix Location : " << viewMatrixLocation << std::endl;

    vec3_f upVector = {0.0, 1.0, 0.0};
    vec3_f eyePosition = {0.0, 0.0, -1.0};
    vec3_f viewVector = {0.0, 0.0, 1.0};
    vec3_f translateVector = {0.0, 0.0, 0.0};
    mat4x4_f modelMatrix;
    modelMatrix = translate(modelMatrix, translateVector);
    std::cout << "Model Matrix : " << modelMatrix << std::endl;
    mat4x4_f viewMatrix = lookAt(eyePosition, viewVector, upVector);
    std::cout << "View Matrix : " << viewMatrix << std::endl;
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
	glDrawArrays(GL_TRIANGLES, 0, 3);

	gc.swapBuffers();
    system("pause");
}