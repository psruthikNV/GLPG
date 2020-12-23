#ifdef _WIN32
#include <Windows.h>
#endif

#include <thread>

#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "utils/GLPGShaderUtils.hpp"
#include "math/GLPGMath.hpp"
#include "utils/GLPGUtils.hpp"
#include "GLPGEvent.hpp"

static bool game_is_running = true;
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

// Who needs mutexes?
struct SharedGameState
{
    float camX;
    float camZ;
};

SharedGameState GLPGState;

void GLPGRenderLoop()
{
    GLPGContext gc;
    GLPGWindow win(800, 600);
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    GLuint modelMatrixLocation = 0;
    GLuint cameraMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    GLuint projectionMatrixLocation = 0;
    GLPGEventLoop eventLoop;
    GLPGCamera camera;
    
    std::vector<VertexIN> monkeyVertices;
    std::vector<FaceIN> faceStuff;

    // Load the vertices of the monkey obj
    const char *monkeyPath = "C:\\Users\\psrut\\repos\\GLPG\\assets\\monkey.obj";

    if (!LoadObjFile(monkeyPath, monkeyVertices, faceStuff)) {
        std::cout << "Failed to load Vertices\n";
        return;
    }

    std::vector<uint32_t> vtxIdx;
    std::cout << "Monkey Vertices sizee : " << monkeyVertices.size() << "\n";
    std::cout << "Face Stuff size : " << faceStuff.size() << "\n";
    std::cout << "vertexIndices size : " << faceStuff[0].vertexIndices.size() << "\n";
    uint32_t counter = 0U;

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

    if (!win.createNativeWindow()) {
        std::cout << "Failed to create native window" << std::endl;
        return;
    }

    if (!gc.initializeGlContext(win, 4, 5)) {
        std::cout << "Failed to initialize GL Context" << std::endl;
        return;
    }

    vtxShaderObj = glCreateShader(GL_VERTEX_SHADER);
    fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    programObj = glCreateProgram();

    if (!compileShader(vtxShaderObj, vertexSource)) {
        std::cout << "Failed to compile GLSL Vertex Shader" << std::endl;
        return;
    }

    if (!compileShader(fragShaderObj, fragmentSource)) {
        std::cout << "Failed to compile GLSL Fragment Shader" << std::endl;
        return;
    }

    glAttachShader(programObj, vtxShaderObj);
    glAttachShader(programObj, fragShaderObj);

    if (!linkShaders(programObj)) {
        std::cout << "Failed to link GLSL Shaders" << std::endl;
        return;
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
    mat4x4_f view;
    SYSTEMTIME sysTime;
    LARGE_INTEGER timer;
    _LARGE_INTEGER test;

    modelMatrix = translate(modelMatrix, translateVector);
    
    glClearColor(0.0, 1.0, 1.0, 1.0);
    if (!eventLoop.SetActiveCamera(&camera)) {
        std::cout << "Failed to set active camera\n";
    }
    modelMatrix.identity();
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
    GLPGEvent event;
    while((event = eventLoop.GetEvent()) != GLPGEvent::WindowClose) {
        switch (event)
        {
            case GLPGEvent::Key_W :
            case GLPGEvent::UpArrow :
                camera.position[2]--;
                break;
            case GLPGEvent::Key_A :
            case GLPGEvent::LeftArrow :
                camera.position[0]--;
                break;
            case GLPGEvent::Key_S :
            case GLPGEvent::DownArrow :
            camera.position[2]++;
                break;
            case GLPGEvent::Key_D :
            case GLPGEvent::RightArrow :
            camera.position[0]++;
                break;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        view = lookAt(camera.position, camera.viewVector, camera.upVector);
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_TRUE, view.data());
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix.data());
        glDrawElements(GL_TRIANGLES, vtxIdx.size(), GL_UNSIGNED_INT, 0);
	    gc.swapBuffers();
    }
    game_is_running = false;
}

// This thread updates 60 times per second.
// This still does not seem to provide time independent render updates
void GLPGUpdateLoop()
{
    LARGE_INTEGER timer;

    const uint32_t UpdatesPerSecond = 60U;
    const uint32_t Skip = 1000U / UpdatesPerSecond;

    DWORD next_game_tick = GetTickCount();

    int sleep_time = 0;
    const float radius = 10.0f;
    
    while( game_is_running ) {
        QueryPerformanceCounter(&timer);
        GLPGState.camX = sin(timer.QuadPart * 0.0000001F) * radius;
        GLPGState.camZ = cos(timer.QuadPart * 0.0000001F) * radius;
        next_game_tick += Skip;
        sleep_time = next_game_tick - GetTickCount();
        if( sleep_time >= 0 ) { 
            Sleep( sleep_time );
        }
    }
}

int main(int argc, char **argv)
{
    std::thread GLPGRenderThread(GLPGRenderLoop);
    std::thread GLPGUpdateThread(GLPGUpdateLoop);

    GLPGRenderThread.join();
    GLPGUpdateThread.join();

    return 0;
}
