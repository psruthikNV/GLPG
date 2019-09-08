/*
 * 04 - Camera Matrix
 * 
 * This example implements a simple Camera matrix.
 * 
 * To implement the necessary transformations required to have the notion of a 
 * movable camera, we have the concept of a camera matrix.
 *
 * In OpenGL and any rendering API for that matter, there is no "camera"
 * as we generally think of it. A camera in the graphics world is just a construct
 * that helps us to implement transformations so that we get the result of having a camera
 * and the models in the scene being rendered through the lens of the camera.
 * 
 * The words camera and eye here have no difference other than the spelling.
 * The coordinates that are obtained after multiplying the models with a model matrix
 * are known as the world coordinates. This will be the world space.
 *
 * To get the effect of looking through a camera, we need to know what the coordinates of the models
 * in the world are in terms of the camera. These coordinates are known as the camera or eye coordinates.
 *
 * A camera has a set place in the world. This place is described in terms of the world coordinates
 * and becomes the camera coordinates or eye coordinates.
 * A camera is also pointed in some direction in the world. Ideally, we'd like the camera to point towards the scene
 * which in OpenGL is -z. This becoes the view vector.
 * Finally, we need to specify the orientation of the camera because with just the eye and view vector, we will not
 * have enough information about the orientation in which the scene must be rendered (upside down / right way up)
 *
 * With the above information, we can create a set of orthonormal vectors that define the camera space.
 * Now, we can create a camera matrix where all the models are transformed w.r.t the position and orientation
 * of the camera. This will lead the coordinates of the models being rendered at positions as if we were looking at
 * them through a camera pointing at a specific direction.
 */

#include "native_window.hpp"
#include "opengl_context.hpp"
#include "utils/opengl_shader_utils.hpp"
#include "math/glpg_math.hpp"
#include "utils/misc_utils.hpp"

using namespace glpg;

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

    vec3_f upVector = {1.0, 0.0, 0.0};
    vec3_f eyePosition = {0.0, 0.0, -1.0};
    vec3_f viewVector = {0.0, 0.0, 1.0};
    vec3_f translateVector = {0.0, 0.0, 0.0};

    mat4x4_f modelMatrix;
    modelMatrix = translate(modelMatrix, translateVector);
    mat4x4_f viewMatrix = lookAt(eyePosition, viewVector, upVector);
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, viewMatrix.data());
	glDrawArrays(GL_TRIANGLES, 0, 3);

	gc.swapBuffers();

    glpg::pause();
}
