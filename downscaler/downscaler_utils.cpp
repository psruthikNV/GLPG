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

#include "downscaler_utils.hpp"

#include <iostream>

namespace downscaler
{

namespace utils
{

void InputParser::printHelpMessage() noexcept
{
    std::cout << "\t Downscaler - Simple image downscaler\n\n\n";
    std::cout << "Mandatory input arguments to be specified:\n\n";
    std::cout << "-inputFile:         Input file containing RGB data to be converted\n";
    std::cout << "-inputFileFormat:   Format of the input file\n";
    std::cout << "                    Valid values: ppm, raw, png\n";
    std::cout << "                    NOTE: Not all file formats may be supported for input file\n";
    std::cout << "-downscalingFactor: The factor of downscaling to be performed\n";
    std::cout << "-windowWidth:       Width of the window used to display\n";
    std::cout << "-windowHeight:      Height of the window used to display\n";
    std::cout << "-help:              Print this help message\n";
}

FileFormat InputParser::toFileFormat(const std::string &inputString) noexcept
{
    FileFormat ret { };

    if (inputString.empty()) {
        ret = FileFormat::unspecified;
    } else if (inputString == "ppm") {
        ret = FileFormat::ppm;
    } else if (inputString == "raw") {
        ret = FileFormat::raw;
    } else if (inputString == "png") {
        ret = FileFormat::png;
    } else {
        ret = FileFormat::unrecognized;
    }

    return ret;
}

Filter InputParser::toFilter(const std::string &inputString) noexcept
{
    Filter ret { };

    if (inputString.empty()) {
        ret = Filter::unspecified;
    } else if (inputString == "nearest") {
        ret = Filter::nearest;
    } else if (inputString == "lanczos") {
        ret = Filter::lanczos;
    } else {
        ret = Filter::unrecognized;
    }

    return ret;
}

utils::InputArguments InputParser::parseArgs(const std::int32_t argc, char **argv)
{
    utils::InputArguments ret { };

    for (std::uint32_t idx { 1U }; idx < static_cast<std::uint32_t>(argc); ++idx)
    {
        if (!strcmp(argv[idx], "-help")) {
            printHelpMessage();
        } else if (!strcmp(argv[idx], "-inputFile") && (idx != argc - 1U)) {
            ret.inputFile = argv[++idx];
        } else if (!strcmp(argv[idx], "-inputFileFormat") && (idx != argc - 1U)) {
            ret.inputFileFormat = toFileFormat(std::string(argv[++idx]));
        } else if (!strcmp(argv[idx], "-downscalingFactor") && (idx != argc - 1U)){
            ret.downscalingFactor = strtol(argv[++idx], nullptr, 10U);
        } else if (!strcmp(argv[idx], "-windowWidth") && (idx != argc - 1U)){
            ret.windowWidth = strtol(argv[++idx], nullptr, 10U);
        } else if (!strcmp(argv[idx], "-windowHeight") && (idx != argc - 1U)){
            ret.windowHeight = strtol(argv[++idx], nullptr, 10U);
        } else if (!strcmp(argv[idx], "-outputImageWidth") && (idx != argc - 1U)){
            ret.outputImageWidth = strtol(argv[++idx], nullptr, 10U);
        } else if (!strcmp(argv[idx], "-outputImageHeight") && (idx != argc - 1U)){
            ret.outputImageHeight = strtol(argv[++idx], nullptr, 10U);
        } else if (!strcmp(argv[idx], "-filter") && (idx != argc - 1U)) {
            ret.filter = toFilter(std::string(argv[++idx]));
        } else if (!strcmp(argv[idx], "-useGpu")) {
            ret.useGpu = true;
        } else if (!strcmp(argv[idx], "-lanczosKernelSize") && (idx != argc - 1U)){
            ret.lanczosKernelSize = strtol(argv[++idx], nullptr, 10U);
        } else {
            throw std::invalid_argument("Unrecognized argument specified! Run with '-help' to see the list of accepted arguments.");
        }
    }

    return ret;
}

void InputParser::verifyArgs(const utils::InputArguments &args)
{
    if (args.inputFile.empty()) {
        throw std::invalid_argument("No input file specified!");
    }

    if (args.inputFileFormat == FileFormat::unrecognized) {
        throw std::invalid_argument("Unrecognized input file format specified");
    }

    if (args.inputFileFormat == FileFormat::unspecified) {
        throw std::invalid_argument("No input file format specified");
    }

    if (args.downscalingFactor == 0U) {
        throw std::invalid_argument("No downscaling factor specified");
    }

}

utils::InputArguments InputParser::parseAndVerifyArgs(std::int32_t argc, char **argv)
{
    try
    {
        utils::InputArguments args { parseArgs(argc, argv) };
        verifyArgs(args);

        return args;
    }
    catch (...)
    {
        // Re-throw caught exception
        throw;
    }
}

} // namespace utils

} // namespace downscaler

