// Copyright (c) 2024 Sruthik P
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Windows.h>

#include <iostream>
#include <stdexcept>

#include <GL/gl.h>
#include <GL/glext.h>

#include "downscaler_utils.hpp"
#include "downscaler_decoder.hpp"
#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "GLPGEvent.hpp"
#include "utils/GLPGShaderUtils.hpp"

const float vertexData[] = {
    -1.0F, -1.0F, 0.0F, 1.0F,
     1.0F, -1.0F, 1.0F, 1.0F,
     1.0F, 1.0F, 1.0F, 0.0F,
     -1.0F, -1.0F, 0.0F, 1.0F,
     1.0F, 1.0F, 1.0F, 0.0F,
     -1.0F, 1.0F, 0.0F, 0.0F
};

const char *vertexShaderSource =
    "#version 450 core\n"
    "layout (location = 0) in vec2 vertexPosition;\n"
    "layout (location = 1) in vec2 textureCoords;\n"
    "out vec2 texCoords;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(vertexPosition, 0.0, 1.0);\n"
    "   texCoords = textureCoords;\n"
    "}\0";
const char *fragmentShaderSource =
    "#version 450 core\n"
    "out vec4 fragmentColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "   fragmentColor = texture2D(texture, texCoords);\n"
    "}\0";

int main(int argc, char **argv)
{

    try
    {
        // Exception aware code beings. No error checks required
        const downscaler::utils::InputArguments args { downscaler::utils::InputParser::parseAndVerifyArgs(argc, argv) };
        downscaler::Decoder decoder(args.inputFile, args.inputFileFormat);
        decoder.init();
        auto decodedData { decoder.decode() };
        decoder.deinit();
        // Exception aware code ends. Error checking required.
        GLuint VBO = 0;
        GLuint VAO = 0;
        GLuint vtxShaderObj = 0;
        GLuint fragShaderObj = 0;
        GLuint programObj = 0;
        GLuint textureID = 0;
        GLPG::GLPGContext context;
        GLPG::GLPGEventLoop eventLoop;
        GLPG::GLPGEvent event;

        GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
        if (!window) {
            std::cerr << "Failed to create GLPGWindow\n";
            return -1;
        }

        if (window->CreateWindow(args.windowWidth, args.windowHeight)) {
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
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glClearColor(0.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, decoder.getWidth(), decoder.getHeight(),0, GL_RGB, GL_UNSIGNED_BYTE, decodedData.data());
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
        context.SwapBuffers();

        while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
            switch(event) {
                case GLPG::GLPGEvent::Key_1:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                case GLPG::GLPGEvent::Key_2:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case GLPG::GLPGEvent::Key_3:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case GLPG::GLPGEvent::Key_4:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
            }
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            context.SwapBuffers();
        }
    }
    catch (std::invalid_argument& e)
    {
        std::cerr << "downscaler: FATAL EXCEPTION: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

