#ifndef GAME_TUTORIAL_OPENGL_HPP
#define GAME_TUTORIAL_OPENGL_HPP

#include <Windows.h>
#include <gl/GL.h>

#include "third_party/opengl/glext.h"
#include "third_party/opengl/wglext.h"

/// Creatre function pointer objects
inline ::PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{};
inline ::PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{};
inline ::PFNGLCREATESHADERPROC glCreateShader{};
inline ::PFNGLDELETESHADERPROC glDeleteShader{};
inline ::PFNGLSHADERSOURCEPROC glShaderSource{};
inline ::PFNGLCOMPILESHADERPROC glCompileShader{};
inline ::PFNGLGETSHADERIVPROC glGetShaderiv{};
inline ::PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog{};
inline ::PFNGLCREATEPROGRAMPROC glCreateProgram{};
inline ::PFNGLDELETEPROGRAMPROC glDeleteProgram{};
inline ::PFNGLATTACHSHADERPROC glAttachShader{};
inline ::PFNGLLINKPROGRAMPROC glLinkProgram{};
inline ::PFNGLGENVERTEXARRAYSPROC glGenVertexArrays{};
inline ::PFNGLGENBUFFERSPROC glGenBuffers{};

#endif //GAME_TUTORIAL_OPENGL_HPP