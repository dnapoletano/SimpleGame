#include <iostream>
#include <print>


#include "auto_release.hpp"
#include "error.h"
#include "exception.hpp"
#include "opengl.hpp"
#include "window.hpp"

namespace {
static constexpr auto vertex_shader = R"(
#version 120

attribute vec3 position;
attribute vec3 colour;

varying vec4 FragColor;

void main(){
   gl_Position = vec4(position, 1.0);
   FragColor = vec4(colour,1.0);
}
)";

static constexpr auto fragment_shader = R"(
#version 120

uniform vec3 vertex_colour;

void main(){
   gl_FragColor = vec4(vertex_colour,1.0);
}
)";
}

auto compile_shader(const std::string_view& src,
   const ::GLenum shader_type) -> game::AutoRelease<::GLuint> {
   auto shader = game::AutoRelease<::GLuint>{
      ::glCreateShader(shader_type),
      ::glDeleteShader
   };
   const ::GLchar* strings[] = {src.data()};
   const ::GLint lengths[] = {static_cast<::GLint>(src.length())};
   ::glShaderSource(shader.get(), 1, strings, lengths);
   ::glCompileShader(shader.get());

   ::GLint result{};
   ::glGetShaderiv(shader.get(),GL_COMPILE_STATUS,&result);
   if (result == GL_FALSE) {
      GLint logSize = 0;
      ::glGetShaderiv(shader.get(), GL_INFO_LOG_LENGTH, &logSize);
      std::vector<::GLchar> glShaderLog(logSize);
      ::glGetShaderInfoLog(shader.get(),logSize,&logSize,&glShaderLog[0]);
      std::ranges::copy(glShaderLog, std::ostream_iterator<char>(std::cout, ""));
   }

   game::ensure(result!=GL_FALSE,
      std::format("failed to compile shader"));
   return shader;
}

auto main() -> int {
   try {
      const game::Window win(600u,400u);
      const auto vshader = compile_shader(vertex_shader,GL_VERTEX_SHADER);
      const auto fshader = compile_shader(fragment_shader,GL_FRAGMENT_SHADER);
      const auto program = game::AutoRelease<::GLuint> {
         ::glCreateProgram(),
         ::glDeleteProgram
      };
      game::ensure(static_cast<bool>(program),
         "Couldn't init opengl program");
      ::glAttachShader(program.get(),vshader.get());
      ::glAttachShader(program.get(),fshader.get());
      ::glLinkProgram(program.get());

      while (win.running()) {

         glClear(GL_COLOR_BUFFER_BIT);
         win.swap();
      }
   } catch (const game::Exception& exception) {
      std::println(std::cerr,"{}",exception);
   } catch (...) {
      std::println(std::cerr,"Unkown exception 🥲");
   }
   return 0;
}
