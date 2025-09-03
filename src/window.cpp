#include <print>


#include "error.h"
#include "opengl.hpp"
#include "window.hpp"


namespace {
   auto g_running = true;

   auto CALLBACK window_proc(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam) -> LRESULT {
      switch (Msg) {
         case WM_CLOSE:
            g_running = false;
            break;
         case WM_KEYDOWN:
            std::println("key down pressed");
            break;
         default:
            return DefWindowProc(hWnd,Msg,wParam,lParam);
      }
      return DefWindowProc(hWnd,Msg,wParam,lParam);
   }
}

namespace game {

Window::Window(const std::uint32_t width, const std::uint32_t height)
   : _wc({}),_win({}), _dc({})
{
   ::RECT rect{.left = {},.top = {},
      .right = static_cast<int>(width),
      .bottom = static_cast<int>(height)};
   ensure(::AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW | WS_VISIBLE,false) !=0,"Couldn't resize window");

   _wc = {
         .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
         .lpfnWndProc = ::window_proc,
         .hInstance = ::GetModuleHandleA(nullptr),
         .lpszClassName = "window class"
   };

   ensure((::RegisterClassA(&_wc) != 0),"Couldn't register class");

   _win = {
      CreateWindowExA( 0, _wc.lpszClassName, "test-window",
         WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right-rect.left,
         rect.bottom-rect.top, nullptr, nullptr,  _wc.hInstance, nullptr)
      , ::DestroyWindow};

   ensure(static_cast<bool>(_win),"could not init window");

   _dc = {
      ::GetDC(_win.get()),
         [this](auto dc_) {::ReleaseDC(_win.get(),dc_);}};

   ::ShowWindow(_win.get(),SW_SHOW);
   ::UpdateWindow(_win.get());

   _resolve_wgl_functions(_wc.hInstance);
   _init_opengl(_dc.get());
   _resolve_global_opengl_functions();

}

auto Window::running() const -> bool {
   auto message = ::MSG{};
   while (::PeekMessageA(&message,nullptr,0,0,PM_REMOVE) != 0) {
      ::TranslateMessage(&message);
      ::DispatchMessageA(&message);
   }

   return g_running;
}

auto Window::swap() const -> void {
   SwapBuffers(_dc.get());
}

auto Window::_resolve_wgl_functions(::HINSTANCE instance) -> void {
   const auto wc = ::WNDCLASS{
      .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      .lpfnWndProc = ::DefWindowProc,
      .hInstance = instance,
      .lpszClassName = "dummy window"
   };
   ensure(::RegisterClassA(&wc)!=0,
      "Couldn't register dummy class");
   auto dummy_window = AutoRelease<::HWND>{
      ::CreateWindowExA(
         0,
         wc.lpszClassName,
         wc.lpszClassName,
         0,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         nullptr,
         nullptr,
         wc.hInstance,
         nullptr
         ),
      ::DestroyWindow
   };
   ensure(static_cast<bool>(dummy_window),
      "Couldn't create dummy window");
   const auto dc =
      AutoRelease<HDC>{::GetDC(dummy_window.get()),
         [&dummy_window](auto dc_){::ReleaseDC(dummy_window.get(),dc_);}};
   constexpr auto pfd = ::PIXELFORMATDESCRIPTOR{
      .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
      .nVersion = 1,
      .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      .iPixelType = PFD_TYPE_RGBA,
      .cColorBits = 32,
      .cAlphaBits = 8,
      .cDepthBits = 24,
      .cStencilBits = 8,
      .iLayerType = PFD_MAIN_PLANE};

   const auto pixel_format = ::ChoosePixelFormat(dc.get(),&pfd);
   ensure(pixel_format!=0,
      "failed to choose pixel format");
   ensure(::SetPixelFormat(dc.get(),pixel_format,&pfd)==TRUE,
      "Failed to set pixel format");
   const auto context = AutoRelease<::HGLRC>{
      ::wglCreateContext(dc.get()),::wglDeleteContext
   };
   ensure(static_cast<bool>(context),
      "Unable to create OGL context");
   ensure(::wglMakeCurrent(dc.get(),context.get())==TRUE,
      "Couldn't make current context");

   _resolve_gl_functions(wglCreateContextAttribsARB,"wglCreateContextAttribsARB");
   _resolve_gl_functions(wglChoosePixelFormatARB,"wglChoosePixelFormatARB");
   ensure(::wglMakeCurrent(dc.get(),nullptr)==TRUE,
   "Couldn't unbind context");
}

template<class T>
auto Window::_resolve_gl_functions(T &function, const std::string& name) -> void {
   const auto address = ::wglGetProcAddress(name.c_str());
   ensure(address!=nullptr, "Could not resolve {}", name);
   function = reinterpret_cast<T>(address);
}

auto Window::_resolve_global_opengl_functions() -> void {
#define VAR_TO_STRING(x) x,#x
   _resolve_gl_functions(VAR_TO_STRING(glCreateShader));
   _resolve_gl_functions(VAR_TO_STRING(glDeleteShader));
   _resolve_gl_functions(VAR_TO_STRING(glShaderSource));
   _resolve_gl_functions(VAR_TO_STRING(glCompileShader));
   _resolve_gl_functions(VAR_TO_STRING(glGetShaderiv));
   _resolve_gl_functions(VAR_TO_STRING(glGetShaderInfoLog));
   _resolve_gl_functions(VAR_TO_STRING(glCreateProgram));
   _resolve_gl_functions(VAR_TO_STRING(glDeleteProgram));
   _resolve_gl_functions(VAR_TO_STRING(glAttachShader));
   _resolve_gl_functions(VAR_TO_STRING(glLinkProgram));
   _resolve_gl_functions(VAR_TO_STRING(glGenVertexArrays));
   _resolve_gl_functions(VAR_TO_STRING(glGenBuffers));
}

auto Window::_init_opengl(const HDC dc) -> void {
   constexpr int pixel_format_attribs[] {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 32,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB,8,
      0
   };
   auto pixel_format = 0;
   auto num_formats = UINT{};
   ::wglChoosePixelFormatARB(dc,pixel_format_attribs,nullptr,1,&pixel_format,&num_formats);
   ensure(num_formats!=0u,
      std::format("{} -> Couldn't choose pixel format",__PRETTY_FUNCTION__));
   auto pfd = ::PIXELFORMATDESCRIPTOR{};
   ensure(::DescribePixelFormat(dc,pixel_format,sizeof(pfd),&pfd)!=0,
      "Couldn't describe pixel format");
   ensure(::SetPixelFormat(dc,pixel_format,&pfd)==TRUE,
      "Couldn't set pixel format");

   constexpr int gl_attribs[] {
      // WGL_CONTEXT_MAJOR_VERSION_ARB,3,
      // WGL_CONTEXT_MINOR_VERSION_ARB,2,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0
   };
   const auto context = ::wglCreateContextAttribsARB(dc,nullptr,gl_attribs);
   ensure(context!=nullptr,
      "Couldn't create wgl context");
   ensure(::wglMakeCurrent(dc,context) == TRUE,
      "Couldn't make current context");
}

}