#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include "internal/GLPGContextImpl.hpp"
#include "internal/GLPGContextImpl_Platform.hpp"

// Forward declaration of common GL APIs.
// Declared in global namespace so that
// clients can call the APIs directly.
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLGETSTRINGIPROC glGetStringi;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLPATHCOMMANDSNVPROC glPathCommandsNV;
extern PFNGLPATHSTRINGNVPROC glPathStringNV;
extern PFNGLPATHPARAMETERINVPROC glPathParameteriNV;
extern PFNGLPATHPARAMETERFNVPROC glPathParameterfNV;
extern PFNGLSTENCILFILLPATHNVPROC glStencilFillPathNV;
extern PFNGLSTENCILSTROKEPATHNVPROC glStencilStrokePathNV;
extern PFNGLCOVERFILLPATHNVPROC glCoverFillPathNV;
extern PFNGLCOVERSTROKEPATHNVPROC glCoverStrokePathNV;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

namespace GLPG {
    class GLPGContextImpl_GL : public GLPGContextImpl {
        public:
            bool InitializeContextImpl() override final;
            bool SwapBuffers() override final {
                return platformImpl->PlatformSwapBuffers();
            }
        private:
            GLPGContextImpl_Platform *platformImpl = nullptr;
    };
} // namespace GLPG
