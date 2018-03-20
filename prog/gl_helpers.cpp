//Scott Saunders, 10163541
//Provides OpenGL helpers

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "gl_helpers.h"

#include <iostream>
#include <fstream>

//using namespace std;

#define lcase(X)	case(X): std::cout << #X << std::endl; break;


// reports GLFW errors
void ErrorCallback(int error, const char* description)
 {
     std::cout << "GLFW ERROR " << error << ":" << std::endl;
     std::cout << description << std::endl;
 }


void check_glerror(){
	GLenum error = (glGetError());
	switch (error){
		case(GL_NO_ERROR) :
		default :
//			std::cout << GL_NO_ERROR << ":" << error << std::endl;
			return;
		case(GL_INVALID_ENUM):
			std::cout << "GL_INVALID_ENUM" << std::endl;
			break;
		case(GL_INVALID_VALUE):
			std::cout << "GL_INVALID_VALUE" << std::endl;
			break;
		case(GL_INVALID_OPERATION):
			std::cout << "GL_INVALID_OPERATION" << std::endl;
			break;
		case(GL_INVALID_FRAMEBUFFER_OPERATION):
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
			break;
		case(GL_OUT_OF_MEMORY):
			std::cout << "GL_OUT_OF_MEMORY" << std::endl;
			break;
		case(GL_STACK_UNDERFLOW):
			std::cout << "GL_STACK_UNDERFLOW" << std::endl;
			break;
		case(GL_STACK_OVERFLOW):
			std::cout << "GL_STACK_OVERFLOW" <<std::endl;
			break;
	}
}

void check_compile(GLuint vertexShader){
	    GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
		std::string infoLog(maxLength, ' ');
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        std::cout << infoLog << std::endl;
        return;
        }

}

GLFWwindow * glfw_init(int X, int Y, char const * Title){  //Initalizes and returns a GLFW window.

  glfwSetErrorCallback(ErrorCallback);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow * window = glfwCreateWindow(X,Y, Title, nullptr, nullptr);

	glfwMakeContextCurrent(window); //Sets up a OpenGL context

  //Requires OpenGL > 4.3
  glDebugMessageCallback(GL_error_callback, NULL);
	glEnable(GL_DEBUG_OUTPUT);								//DEBUG :D
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS);

	return( window); // Windowed
//	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

}


// TAKEN FROM BOILERPLATE
// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
std::string LoadSource(const std::string &filename)
{
    std::string source;

    std::ifstream input(filename.c_str());
    if (input) {
        copy(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>(),
            back_inserter(source));
        input.close();
    }
    else {
        std::cout << "ERROR: Could not load shader source from file "
            << filename << std::endl;
    }
//	std::cout << "RAW READ: " << source << " : END READ" << std::endl;
    return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const std::string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);

    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        std::string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        std::cout << "ERROR compiling shader:" << std::endl << std::endl;
        std::cout << source << std::endl;
        std::cout << info << std::endl;
    }

    return shaderObject;
}

//  linked from vertex and fragment shaders
GLuint LinkProgram(GLuint programObject)
{


    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        std::string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        std::cout << "ERROR linking shader program:" << std::endl;
        std::cout << info << std::endl;
    }

    return programObject;
}

void check_gllink(GLuint programObject){
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        std::string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        std::cout << "ERROR linking program:" << std::endl;
        std::cout << info << std::endl;
		exit(32);
    }
}








// https://www.opengl.org/wiki/Debug_Output#Getting_messages //

void GL_error_callback(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
	if( true && severity != GL_DEBUG_SEVERITY_NOTIFICATION){

	std::cout << "GL ERROR CALLBACK: " << std::endl;
	std::cout << "Source: " << source << " : ";
	switch (source){
		lcase(GL_DEBUG_SOURCE_API)
		lcase(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
		lcase(GL_DEBUG_SOURCE_SHADER_COMPILER)
		lcase(GL_DEBUG_SOURCE_THIRD_PARTY)
		lcase(GL_DEBUG_SOURCE_APPLICATION)
		lcase(GL_DEBUG_SOURCE_OTHER)				//THESE ARN'T DEFINED?
	}

	std::cout << "Type: " <<  type << " : ";
	switch (type){
		lcase(GL_DEBUG_TYPE_ERROR)
		lcase(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		lcase(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		lcase(GL_DEBUG_TYPE_PORTABILITY)
		lcase(GL_DEBUG_TYPE_PERFORMANCE)
		lcase(GL_DEBUG_TYPE_MARKER)
		lcase(GL_DEBUG_TYPE_PUSH_GROUP)
		lcase(GL_DEBUG_TYPE_POP_GROUP)
		lcase(GL_DEBUG_TYPE_OTHER)
	}

	std::cout << "ID: " << id << std::endl;
	std::cout << "Serverity: " << severity << " : ";
	switch (severity){
		lcase(GL_DEBUG_SEVERITY_HIGH)
		lcase(GL_DEBUG_SEVERITY_MEDIUM)
		lcase(GL_DEBUG_SEVERITY_LOW)
		lcase(GL_DEBUG_SEVERITY_NOTIFICATION)
	}
	std::cout << "Length: " << length << std::endl;
	std::cout << "Message: " << message << std::endl;
	std::cout << "UserParam: " << userParam << std::endl;
	std::cout << "END: GL ERROR CALLBACK: " << std::endl;

	}
}
