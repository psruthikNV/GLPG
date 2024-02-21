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

#pragma once

#include <fstream>
#include <vector>

#include "downscaler_utils.hpp"

namespace downscaler
{

class Downscaler
{
    private:
        std::uint32_t m_inputWidth;
        std::uint32_t m_inputHeight;
        std::uint32_t m_outputWidth;
        std::uint32_t m_outputHeight;
        std::vector<std::uint8_t> &m_decodedData;
        std::vector<std::uint8_t> m_downscaledData;
        utils::Filter m_filter;

        std::uint32_t pixelLocationToIndex(std::uint32_t xLocation, std::uint32_t yLocation);
        void nearestNeighbour();
        void lanczos();
        std::tuple<uint8_t, uint8_t, uint8_t> lanczosResample(double x, double y);
    public:
        Downscaler(std::uint32_t inputWidth, std::uint32_t inputHeight,
                   std::uint32_t outputWidth, std::uint32_t outputHeight,
                   std::vector<std::uint8_t> &decodedData,
                   utils::Filter filter) :
            m_inputWidth(inputWidth),
            m_inputHeight(inputHeight),
            m_outputWidth(outputWidth),
            m_outputHeight(outputHeight),
            m_decodedData(decodedData),
            m_filter(filter) { }

        std::vector<std::uint8_t> &downscale();
};

} // namespace downscaler
