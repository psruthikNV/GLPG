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

#include <cstdint>
#include <string>

namespace downscaler
{

namespace utils
{

///
/// @brief Supported file formats
///
enum class FileFormat : std::uint32_t
{
    unspecified = 0U, ///< Default file format type.
    ppm, ///< PPM format
    raw, ///< Raw bytes representing the image contents
    png, //< PNG format
    unrecognized, ///< Unrecognized file format type
    last = unrecognized
};

///
/// @brief Input arguments specified by the client
///
struct InputArguments
{
    std::string inputFile; ///< The input file containing image data to be converted
    FileFormat inputFileFormat; ///< The file format of @ref InputArguments::inputFile
    std::uint32_t downscalingFactor; ///< The factor of downscaling to be done
    std::uint32_t windowWidth; ///< Width of window on which the image will be displayed
    std::uint32_t windowHeight; ///< Width of window on which the height will be displayed
};

///
/// @brief Helper class to query and validate input arguments
///
class InputParser
{
    private:
        ///
        /// @brief Converts the input std::string to @ref FileFormat
        ///
        /// @param[in] inputString String to be converted
        ///
        /// Design:
        /// -# Perform the conversion as below:
        ///    -# Input string: Empty string - Return @ref FileFormat::unspecified
        ///    -# Input string: ppm - Return @ref FileFormat::ppm
        ///    -# Input string: raw - Return @ref FileFormat::raw
        ///    -# Input string: png - Return @ref FileFormat::png
        ///    -# Input string: None of the above - Return @ref FileFormat::unrecognized
        ///
        /// @returns @ref FileFormat converted from @p inputString
        ///
        static FileFormat toFileFormat(const std::string &inputString) noexcept;

        ///
        /// @brief Verifies the input @p args
        ///
        /// @param[in] args The @ref InputArguments to be verified
        ///
        /// @throws std::invalid_argument If the input @p args fails verification
        ///
        /// Design:
        /// -# Do not throw an exception if the following are true:
        ///    -# @ref InputArguments::inputFile is not empty
        ///    -# @ref InputArguments::inputFileFormat is not @ref FileFormat::unrecognized and @ref FileFormat::unspecified
        ///    -# @ref InputArguments::downscalingFactor is not empty
        /// -# Else, throw std::invalid_argument exception
        ///
        static void InputParser::verifyArgs(const utils::InputArguments &args);

        ///
        /// @brief Parse command line arguments and construct a @ref InputArguments object
        ///
        /// @param[in] argc Number of command line arguments specified
        /// @param[in] argv Command line arguments
        ///
        /// @throws std::invalid_argument if an unrecognized argument is present
        ///
        /// Design:
        /// -# Parse the following input arguments:
        ///    -# Argument: -help
        ///       -# Invoke @ref InputParser::printHelpMessage
        ///    -# Argument: -inputFile
        ///       -# Verify that a value for the argument is specified and store the argument
        ///          in @ref InputArguments::inputFile
        ///    -# Argument: -inputFileFormat
        ///       -# Verify that a value for the argument is specified and store the argument
        ///          in @ref InputArguments::inputFileFormat
        ///          (Note: Use @ref InputParser::toFileFormat)
        ///    -# Argument: -downscalingFactor
        ///       -# If specified, set @ref InputArguments::disableSimd to @true
        ///    -# Argument: None of the above
        ///       -# Throw std::invalid_argument
        /// -# Construct and return a pair of @c parseError and the constructed @ref InputArguments
        ///
        /// @returns @ref utils::InputArguments containing verified input values
        ///
        static utils::InputArguments InputParser::parseArgs(const std::int32_t argc, char **argv);

        ///
        /// @brief Print a help message on stdout
        ///
        /// Design:
        /// -# Print to stdout a help message describing the features and input arguments
        ///    of downscaler
        ///
        /// @returns None
        ///
        static void printHelpMessage() noexcept;

    public:
        ///
        /// @brief Parse and verify specified command line arguments
        ///
        /// @param[in] argc Number of command line arguments specified
        /// @param[in] argv Command line arguments
        ///
        /// @throws std::invalid_argument (Indirectly via @ref InputParser::parseArgs or @ref InputParser::verifyArgs)
        ///
        /// Design:
        /// -# Invoke @ref InputParser::parseArgs to parse command line arguments.
        /// -# If @ref InputParser::parseArgs passes (returns @true):
        ///    -# Invoke @ref InputParser::verifyArgs
        ///    -# Return a pair of the result of the above step and @ref InputArguments returned by @ref InputParser::parseArgs
        /// -# Else:
        ///    -# Return a pair of @false and @ref InputArguments returned by @ref InputParser::parseArgs
        ///
        /// @returns @ref InputArguments containing the parsed and verified input arguments
        ///
        static utils::InputArguments InputParser::parseAndVerifyArgs(int32_t argc, char **argv);
};

} // namespace utils

} // namespace downscaler

