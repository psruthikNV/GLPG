#include "utils/native_window.hpp"
#include "utils/opengl_context.hpp"
#include <vector>
#include "utils/opengl_loader.hpp"
bool compileShader(int shaderObject, const char *shaderSource)
{
    int success = GL_FALSE;
    glShaderSource(shaderObject, 1, &shaderSource, nullptr);
    glCompileShader(shaderObject);
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        int shaderType;
        int maxLength = 0;
        glGetShaderiv(shaderObject, GL_SHADER_TYPE, &shaderType);
        std::cout << "Shader compilation of type " << shaderType << " Failed!!"
                  << std::endl;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<char> errorLog(maxLength);
        glGetShaderInfoLog(shaderObject, maxLength, &maxLength, &errorLog[0]);
        //glDeleteShader(shaderObject);
        std::string s(errorLog.begin(), errorLog.end());
        std::cout << "Compilation Log: " << s << std::endl;
        
        return false;
    }
    return true;
}

bool linkShaders(int programObject)
{
    int success = GL_FALSE;

    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &success);

    if (success != GL_TRUE) {
        int maxLength = 0;
        std::cout << "Shader Linking Failed !!" << std::endl;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<char> errorLog(maxLength);
        glGetProgramInfoLog(programObject, maxLength, &maxLength, &errorLog[0]);

        std::string s(errorLog.begin(), errorLog.end());
        std::cout << "Link Log : " << s << std::endl;
        return false;
    }
    return true;
}

const float vertexData[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
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
    nativeWindow win(640, 480);
    glContext gc;
    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint vtxShaderObj = 0;
    GLuint fragShaderObj = 0;
    GLuint programObj = 0;
    int success = 0;

    if (!win.createNativeWindow()) {
        std::cout << "Failed to create Native Window" << std::endl;
        return -1;
    }

    if (!gc.initializeGlContext(win, 4, 6)) {
        std::cout << "Failed to initialize GL Context" << std::endl;
        return -1;
    }
    loadGLFunctions();

    vtxShaderObj = glCreateShader(GL_VERTEX_SHADER);
    fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    programObj = glCreateProgram();
    if (!compileShader(vtxShaderObj, vertexShaderSource)) {
        std::cout << "Vertex Shader Compilation Failed" << std::endl;
        return -1;
    }
    if (!compileShader(fragShaderObj, fragmentShaderSource)) {
        std::cout << "Fragment Shader Compilation Failed" << std::endl;
        return -1;
    }

    glAttachShader(programObj, vtxShaderObj);
    glAttachShader(programObj, fragShaderObj);

    if (!linkShaders(programObj)) {
        std::cout << "Failed to link Shaders" << std::endl;
        return -1;
    }

    glUseProgram(programObj);
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
    gc.swapBuffers();
    system("pause");
    return 0;
}
