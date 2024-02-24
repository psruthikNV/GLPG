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

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdexcept>

#include <GL/gl.h>
#include <GL/glext.h>

#include "downscaler.hpp"
#include "downscaler_utils.hpp"
#include "downscaler_decoder.hpp"
#include "GLPGWindow.hpp"
#include "GLPGContext.hpp"
#include "GLPGEvent.hpp"
#include "utils/GLPGShaderUtils.hpp"

namespace downscaler
{

std::uint32_t Downscaler::pixelLocationToIndex(std::uint32_t xLocation, std::uint32_t yLocation)
{
    return (yLocation * m_inputWidth * 3U) + xLocation * 3U;
}


static double lanczosKernel(double x)
{
    if (x == 0.0) {
        return 1.0;
    }
    if (std::fabs(x) >= 5U) {
        return 0.0;
    }

    double ret { std::sin(M_PI * x) * std::sin(M_PI * x / 5U) / (M_PI * M_PI * x * x / 5U) };
    return ret;
}

std::tuple<uint8_t, uint8_t, uint8_t> Downscaler::lanczosResample(double x, double y)
{
    int ix, iy;
    double r = 0.0, g = 0.0, b = 0.0;
    double weights { 0.0 };

    for (ix = (int)(x - 5U + 1); ix <= (int)(x + 5U); ix++) {
        for (iy = (int)(y - 5U + 1); iy <= (int)(y + 5U); iy++) {
            if (ix >= 0 && ix < m_inputWidth && iy >= 0 && iy < m_inputHeight) {
                double weight = lanczosKernel(x - ix) * lanczosKernel(y - iy);
                r += weight * m_decodedData[pixelLocationToIndex(ix, iy)];
                g += weight * m_decodedData[pixelLocationToIndex(ix, iy) + 1U];
                b += weight * m_decodedData[pixelLocationToIndex(ix, iy) + 2U];
                weights += weight;
            }
        }
    }

    return std::make_tuple((uint8_t)(r / weights), (uint8_t)(g / weights), (uint8_t)(b / weights));
}

void Downscaler::lanczos()
{
    double scaleX { double(m_inputWidth)/double(m_outputWidth) };
    double scaleY { double(m_inputHeight)/double(m_outputHeight) };
    double invScale { scaleX };

    for (std::uint32_t y { 0U }; y < m_outputHeight; ++y) {
        for (std::uint32_t x { 0U }; x < m_outputWidth; ++x) {
            double originalX { x * invScale };
            double originalY { y * invScale };
            auto ret { lanczosResample(originalX, originalY) };
            m_downscaledData.emplace_back(std::get<0>(ret));
            m_downscaledData.emplace_back(std::get<1>(ret));
            m_downscaledData.emplace_back(std::get<2>(ret));
        }
    }
    std::cout << "P3\n";
    std::cout << m_outputWidth << " " << m_outputHeight << "\n";
    std::cout << "255\n";
    uint32_t idx = 0U;
    for (const std::uint8_t value : m_downscaledData)
    {
        ++idx;
        std::cout << static_cast<std::uint32_t>(value);
        if (idx % 3U == 0U) {
            std::cout << "\n";
        } else {
            std::cout << " ";
        }
    }

}

void Downscaler::nearestNeighbour()
{
    const std::uint32_t downscalingFactor { m_inputWidth / m_outputWidth };
    if (downscalingFactor != (m_inputHeight / m_outputHeight)) {
        throw std::logic_error("Unsupported downscaling factor");
    }

    for (std::uint32_t y { 0U }; y < m_outputHeight; ++y) {
        for (std::uint32_t x { 0U }; x < m_outputWidth; ++x) {
            std::uint32_t originalX { x * downscalingFactor };
            std::uint32_t originalY { y * downscalingFactor };
            m_downscaledData.emplace_back(m_decodedData[pixelLocationToIndex(originalX, originalY)]);
            m_downscaledData.emplace_back(m_decodedData[pixelLocationToIndex(originalX, originalY) + 1U]);
            m_downscaledData.emplace_back(m_decodedData[pixelLocationToIndex(originalX, originalY) + 2U]);
        }
    }

    std::cout << "P3\n";
    std::cout << m_outputWidth << " " << m_outputHeight << "\n";
    std::cout << "255\n";
    std::uint32_t idx = 0U;
    for (const std::uint8_t value : m_downscaledData)
    {
        ++idx;
        std::cout << static_cast<std::uint32_t>(value);
        if (idx % 3U == 0U) {
            std::cout << "\n";
        } else {
            std::cout << " ";
        }
    }
}

std::vector<std::uint8_t>& Downscaler::downscale()
{
    switch (m_filter)
    {
        case utils::Filter::nearest:
            nearestNeighbour();
            break;
        case utils::Filter::lanczos:
            lanczos();
            break;
        default:
            throw std::logic_error("Unsupported downscaling filter");
            break;
    }

    return m_downscaledData;
}

} // namespace downscaler
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

const char *nearestFragmentShaderSource =
    "#version 450 core\n"
    "out vec4 fragmentColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "   fragmentColor = texelFetch(texture, ivec2(gl_FragCoord.x * 4, gl_FragCoord.y * 4), 0);\n"
    "}\0";

const char *lanczosFragmentShaderSource =
    "#version 450 core\n"
    "out vec4 fragmentColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "   fragmentColor = texelFetch(texture, ivec2(gl_FragCoord.x * 4, gl_FragCoord.y * 4), 0);\n"
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
        downscaler::Downscaler ds(decoder.getWidth(), decoder.getHeight(), args.outputImageWidth, args.outputImageHeight,
                                  decodedData, args.filter);
        auto downscaledData { ds.downscale() };
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

        GLuint nearestFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint nearestProgObj = glCreateProgram();

        if (!GLPG::compileShader(nearestFragShader, nearestFragmentShaderSource)) {
            std::cout << "Nearest Fragment Shader Compilation Failed" << std::endl;
            return -1;
        }

        glAttachShader(nearestProgObj, vtxShaderObj);
        glAttachShader(nearestProgObj,nearestFragShader);

        if (!GLPG::linkShaders(nearestProgObj)) {
            std::cout << "Failed to link nearest Shaders" << std::endl;
            return -1;
        }

        glUseProgram(programObj);
        //glUseProgram(nearestProgObj);
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

