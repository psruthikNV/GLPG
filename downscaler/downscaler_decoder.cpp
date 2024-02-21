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

#include <iostream>
#include <tuple>
#include "downscaler_decoder.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace downscaler
{

void Decoder::init()
{
    if (!isSupported(m_inputFileFormat)) {
        throw std::invalid_argument("Input file format not supported for decoding!");
    }

    m_inputFileStream = std::fstream(m_inputFile, std::ios::in);
    if (!m_inputFileStream.is_open()) {
        throw std::invalid_argument("Failed to open input file");
    }
}

void Decoder::deinit()
{
    if (m_inputFileStream.is_open()) {
        m_inputFileStream.close();
    }
}

std::vector<std::uint8_t>& Decoder::decode()
{
    try
    {
        switch (m_inputFileFormat)
        {
            case(utils::FileFormat::ppm):
                decodePpm();
                break;
            case(utils::FileFormat::raw):
                decodeRaw();
                break;
            case(utils::FileFormat::png):
                decodePng();
                break;
            default:
                // Unreachable. Do nothing
                break;
        }
        return m_decodedData;
    }
    catch(...)
    {
        std::cerr << "Decoding failed\n";
        throw;
    }
}

void Decoder::decodePpm()
{
    std::uint32_t maxColorValue { 0U };
    constexpr uint32_t expectedMaxColorValue { 255U };
    // Verify PPM header
    std::string ppmMagic { };
    std::string expectedPpmMagic { "P3" };
    std::getline(m_inputFileStream, ppmMagic);
    if (ppmMagic != expectedPpmMagic)
    {
        throw std::invalid_argument("Unsupported input PPM file. Only ASCII RGB PPM files (P3) are supported");
    }

    // Get width, height and max supported color value from PPM file
    m_inputFileStream >> m_width;
    m_inputFileStream >> m_height;
    m_inputFileStream >> maxColorValue;
    if (maxColorValue != expectedMaxColorValue)
    {
        throw std::invalid_argument("Input PPM file contains a maximum RGB value > 255. Not supported");
    }

    // Get the ASCII RGB values from the file and store them in binary form
    for (std::uint32_t readValue { 0U }; m_inputFileStream >> readValue;)
    {
        m_decodedData.emplace_back(static_cast<std::uint8_t>(readValue));
    }

    // Debug code to verify that the parsed data matches the input file
    // TODO: Remove in final release
#if 0
    std::cout << "P3\n";
    std::cout << width << " " << height << "\n";
    std::cout << maxColorValue << "\n";
    std::uint64_t idx { 0U };
    for (const std::uint8_t value : m_decodedData)
    {
        ++idx;
        std::cout << static_cast<std::uint32_t>(value);
        if (idx % 3U == 0U) {
            std::cout << "\n";
        } else {
            std::cout << " ";
        }
    }
#endif
}

void Decoder::decodeRaw()
{
    // TODO
}

//TODO This implementation depends on stb_image. Replace with a bespoke
//implementation
void Decoder::decodePng()
{
    int32_t numComp { 0 };
    int32_t w, h;
    std::uint8_t *decodedData { stbi_load(m_inputFile.c_str(), &w, &h, &numComp, 0) };
    if (!decodedData) {
        throw std::logic_error("Failed to decode PNG image");
    }
    std::cout << "numComp: " << numComp << "\n";

    for (std::uint32_t idx = 0U; idx < w * h * numComp; idx++) {
        m_decodedData.emplace_back(decodedData[idx]);
    }

    m_width = static_cast<std::uint32_t>(w);
    m_height = static_cast<std::uint32_t>(h);
}

bool Decoder::isSupported(utils::FileFormat fileFormat) noexcept
{
    bool ret { false };

    switch(fileFormat)
    {
        case(utils::FileFormat::ppm):
        case(utils::FileFormat::raw):
        case(utils::FileFormat::png):
            ret = true;
            break;
        default:
            // Do nothing
            break;
    }

    return ret;
}

} // namespace downscaler

