#pragma once
#include "math/GLPGVector.hpp"

namespace GLPG{
    struct GLPGCamera
    {
        vec3_f upVector;
        vec3_f position;
        vec3_f viewVector;

        GLPGCamera() : position({0.0f, 0.0f, 3.0f}), upVector({0.0f, 1.0f, 0.0f}), viewVector({0.0f, 0.0f, 0.0f}) {}
        void SetPosition_X(float pos) { position[0] = pos; }
        void SetPosition_Y(float pos) { position[1] = pos; }
        void SetPosition_Z(float pos) { position[2] = pos; }
    };
}