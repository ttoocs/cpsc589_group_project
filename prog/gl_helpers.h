//Scott Saunders, 10163541
//Provides OpenGL helpers

#pragma once
#include <iostream>
#include <fstream>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#define MYGLUEMARK

#include <vector>
#include <string>

#define lcase(X)	case(X): std::cout << #X << std::endl; break;


// reports GLFW errors
void ErrorCallback(int error, const char* description);

void check_glerror();

void check_compile(GLuint vertexShader);

GLFWwindow * glfw_init(int X, int Y, char const * Title);


// TAKEN FROM BOILERPLATE
// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
extern std::string LoadSourceM(std::vector<std::string> & filenames);
//extern std::string LoadSource(const std::string &filename) { std::cout <<  "todo" << std::endl; }
inline std::string LoadSource(const std::string &filename) { std::vector<std::string> f; f.push_back(filename); return LoadSourceM(f);}

// creates and returns a shader object compiled from the given source
extern GLuint CompileShader(GLenum shaderType, const std::string &source);

//  linked from vertex and fragment shaders
extern GLuint LinkProgram(GLuint programObject);

extern void check_gllink(GLuint programObject);

// https://www.opengl.org/wiki/Debug_Output#Getting_messages //

extern void GL_error_callback(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
