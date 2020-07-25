#pragma once

#include <iostream>
#include <stdint.h>

namespace GLPG {
    class GLPGWindowImpl {
        public:
            virtual bool CreateWindowImpl(uint32_t width,
                                          uint32_t height) = 0;
            virtual uint32_t GetWidthImpl() = 0;
            virtual uint32_t GetHeightImpl() = 0;
            virtual ~GLPGWindowImpl() {} ;
    };
} // namespace GLPG
