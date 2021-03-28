#pragma once
#include "../GLPGEvent_Enums.hpp"

namespace GLPG {
    class GLPGEventImpl {
        public:
            virtual bool CreateEventLoopImpl() = 0;
            virtual GLPGEvent GetEventImpl() = 0;
            virtual ~GLPGEventImpl() {} ;
    };
} // namespace GLPG
