/*
 * 02 - Model matrix
 *
 * This example introduces the concept of model matrices.
 *
 * OpenGL understands only one coordinate space and that is
 * the Normalized Device coordinates. These coordinates range from
 * [-1, 1]. Any fragment that has coordinates outside of this range
 * ends up getting clipped.
 *
 * The Model matrix is unique to each model / set of vertices being rendered.
 * The model will end up having its own coordinates where 0,0,0 would refer to the center
 * of the model. While these coordinates can be directly used by OpenGL by passing them
 * to the vertex shader, when there are multiple models imported into the game world,
 * they all will end up at the same spot overlapping the already existing models in the same spot.
 * 
 * To circumvent that, we have the model matrix. The model matrix can consist of arbitary
 * rotations/translations or any other transformation operation. Ideally, each model will have 
 * a unique model matrix associated with it so that it is at a distinct place in the game world.
 *
 * Once we construct a model matrix, it can be passed to the vertex shader and multiplied with the 
 * position of the vertices of the model to get the final transformed coordinates where the vertices end
 * up getting drawn.
 */

#include <random>
#include <GL/gl.h>
#include <GL/glext.h>

#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "GLPGEvent.hpp"
#include "utils/GLPGShaderUtils.hpp"
#include "math/GLPGMath.hpp"

using namespace GLPG;

// Number of triangles to be drawn
int g_numTriangles = 3;

const float vertexData[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

const char *vertexShaderSource = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "uniform mat4 modelMatrix;\n"
    "void main() {\n"
    "   gl_Position = modelMatrix * vec4(vertexPosition, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = 
    "#version 450 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";


int main(int argc, char **argv)
{
    GLuint VBO;
    GLuint VAO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint modelMatrixLocation;
    GLPGContext context;
    GLPGEventLoop eventLoop;
    GLPGEvent event;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);

    if (argc > 1) {
        g_numTriangles = atoi(argv[1]);
    }

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
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
    glEnableVertexAttribArray(0);
    modelMatrixLocation = glGetUniformLocation(programObj, "modelMatrix");

    glClearColor(0.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        for (uint32_t i = 0; i < g_numTriangles; i++) {
            mat4x4_f modelMatrix;
            // For each triangle being drawn we generate a random translation vector
            // which is then used to create the model matrix.
            vec3_f translateVector = {dist(gen),
                                      dist(gen),
                                      dist(gen)};
            modelMatrix = translate(modelMatrix, translateVector);
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        context.SwapBuffers();
    }
}
