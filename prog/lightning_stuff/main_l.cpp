#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
//#include <GLFW/glfw3.h>
//#include "include/glm/ext.hpp"
#include "glm/ext.hpp"
#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <string>
#include <fstream>
#include <iterator>
#include <sstream>

#include <chrono>
#include <thread>

#include "lightning.h"
#include "bspline.h"

	#define vPrint(X)   "(" << X.x << "," <<  X.y << "," << X.z << ")"

#define GL_SHADER_STORAGE_BUFFER 0x90D2

using namespace std;
using namespace glm;

int screenWidth = 1000;
int screenHeight = 1000;

unsigned int VBO;
unsigned int VAO;
int num_points;

int num_control;
int num_spline;

unsigned int controlVBO;
unsigned int controlVAO;

unsigned int splineVBO;
unsigned int splineVAO;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int program;

unsigned int splineVShader;
unsigned int splineFShader;
unsigned int splineProgram;

GLuint segBuffer;

BSpline spline;
vector<vec3> storage;
bool left_mouse_down = false;
bool editMode = true;

/*
	CAMERA CLASS TAKEN FROM: learnopengl.com
*/

struct Camera
{
	float cameraSpeed = 0.01f;

	float yaw = 270.0f;
	float pitch = 0.0f;

	vec3 cameraPos = vec3(0.0f, 0.0f, 1.0f);
	vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
	vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

	mat4 view()
	{
		vec3 right = normalize(cross(cameraFront, vec3(0.0f, 1.0f, 0.0f)));
		cameraUp = normalize(cross(right, cameraFront));
		return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    
	}
} camera;


bool firstMouse = true;
float lastX = (float) screenWidth / 2.0;
float lastY = (float) screenHeight / 2.0;

vector<Segment> lightning_segs;
vector<float> vertices;

void genVBOsVAOs();
void initLightningProgram();
void initSplineProgram();
void updateCamera();
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void loadPoints();
void loadSpline();
void render();
void loadScreen();


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	genVBOsVAOs();
	initLightningProgram();
	initSplineProgram();
	updateCamera();
  
    spline = BSpline();
    spline.addPoint(vec3(-1.0, 0.0, 0.0));
    spline.addPoint(vec3(0.0, 1.0, 0.0));
    spline.addPoint(vec3(1.0, 0.0, 0.0));

	spline.loadControlPoints();
	spline.loadBSpline();
	loadSpline();
	loadScreen();
	glBindVertexArray(VAO);

	while (!glfwWindowShouldClose(window))
	{
		if ((left_mouse_down) && (editMode))
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			int winWidth, winHeight;
			glfwGetWindowSize(window, &winWidth, &winHeight);

			xpos = (xpos - (winWidth / 2)) / (winWidth / 2);
			ypos = (ypos - (winHeight / 2)) / (-1 * winHeight / 2);

			spline.movePoint(vec3(xpos, ypos, 0.0));
			spline.loadControlPoints();
			spline.loadBSpline();
			loadSpline();
	    lightning::loadPoints(spline);
			
		}
		
		glfwSwapBuffers(window);

		//Render command
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		render();
		//std::this_thread::sleep_for(std::chrono::milliseconds(200));
		glfwPollEvents();
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glfwTerminate();
	return 0;
}

void genVBOsVAOs()
{
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &controlVBO);
	glGenVertexArrays(1, &controlVAO);

	glGenBuffers(1, &splineVBO);
	glGenVertexArrays(1, &splineVAO);
//	glGenBuffers(1, &lightning::segmentBuffer);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,lightning::segmentBuffer);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points
//		glEnableVertexAttribArray(0);

}

void initLightningProgram()
{
	// 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open("vertex_raytrace.glsl");
        fShaderFile.open("scene1FragShader.glsl" /*"fragment_shader.glsl"*/);
        
        std::stringstream vShaderStream, fShaderStream;
        fShaderStream << "#version 430 \r\n";
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::LIGHTNING::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
	
	// Vertex Shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

	// Check if compilation is a success
	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::LIGHTNING::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
		exit(0);
	}


	// Point Mass Fragment Shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::LIGHTINING::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
		exit(0);
	}
	
	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::LIGHTNING::LINKING_FAILED\n" << infoLog << endl;
		exit(0);
	}
}

void initSplineProgram()
{
	// 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open("vertex_raytrace.glsl");
        fShaderFile.open("fragment_shader.glsl");
        
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SPLINE::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
	
	// Vertex Shader
	splineVShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(splineVShader, 1, &vShaderCode, NULL);
	glCompileShader(splineVShader);

	// Check if compilation is a success
	int success;
	char infoLog[512];

	glGetShaderiv(splineVShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(splineVShader, 512, NULL, infoLog);
		cout << "ERROR::SPLINE::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
		exit(0);
	}


	// Point Mass Fragment Shader
	splineFShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(splineFShader, 1, &fShaderCode, NULL);
	glCompileShader(splineFShader);

	glGetShaderiv(splineFShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(splineFShader, 512, NULL, infoLog);
		cout << "ERROR::SPLINE::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
		exit(0);
	}
	
	splineProgram = glCreateProgram();

	glAttachShader(splineProgram, splineVShader);
	glAttachShader(splineProgram, splineFShader);
	glLinkProgram(splineProgram);

	glGetProgramiv(splineProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(splineProgram, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::SPLINE::LINKING_FAILED\n" << infoLog << endl;
		exit(0);
	}
}

void loadScreen(){
	storage.clear();
	storage.push_back(vec3(-1.0, 1.0, 0.0));
	storage.push_back(vec3(1.0, 1.0, 0.0));
	storage.push_back(vec3(1.0, -1.0, 0.0));

	storage.push_back(vec3(1.0, -1.0, 0.0));
	storage.push_back(vec3(-1.0, -1.0, 0.0));
	storage.push_back(vec3(-1.0, 1.0, 0.0));
	glUseProgram(program);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, storage.size() * sizeof(vec3), storage.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	num_points = storage.size();
}

void loadSpline()
{
	glUseProgram(splineProgram);
	glBindVertexArray(controlVAO);

	glBindBuffer(GL_ARRAY_BUFFER, controlVBO);
	glBufferData(GL_ARRAY_BUFFER, spline.control_vecs.size() * sizeof(vec3), spline.control_vecs.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	num_control = spline.control_vecs.size();

	glBindVertexArray(splineVAO);

	glBindBuffer(GL_ARRAY_BUFFER, splineVBO);
	glBufferData(GL_ARRAY_BUFFER, spline.bspline_vecs.size() * sizeof(vec3), spline.bspline_vecs.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	num_spline = spline.bspline_vecs.size();
}

void updateCamera()
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 mvp;

	model = mat4(1.0f);
	view = camera.view();
	projection = perspective(radians(45.0f), (float) screenWidth / screenHeight, 0.01f, 100.0f);

	//mvp = projection * view * model;

	mvp = view;

	glUseProgram(program);

	int mvpLoc = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));

	int cameraPosLoc = glGetUniformLocation(program, "cameraPosition");
	glUniform3f(cameraPosLoc, camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (editMode){
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			spline.decOrder();
			spline.loadBSpline();
			loadSpline();
		}
		else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		{
			spline.incOrder();
			spline.loadBSpline();
			loadSpline();
		}
		else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			spline.decResolution();
			spline.loadBSpline();
			loadSpline();
		}
		else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			spline.incResolution();
			spline.loadBSpline();
			loadSpline();
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			editMode = !editMode;
			glBindVertexArray(VAO);
			lightning::loadPoints(spline);
		}
	}
	/* CAMERA */
	else{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.cameraPos += camera.cameraSpeed * camera.cameraFront;
			updateCamera();
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.cameraPos -= camera.cameraSpeed * camera.cameraFront;
			updateCamera();
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.cameraPos -= normalize(cross(camera.cameraFront, camera.cameraUp)) * camera.cameraSpeed;
			updateCamera();
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.cameraPos += normalize(cross(camera.cameraFront, camera.cameraUp)) * camera.cameraSpeed;
			updateCamera();
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			editMode = !editMode;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

/*
	MOUSE PROCESSING FOR CAMERA TAKEN FROM: learnopengl.com
*/

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (editMode)
	{
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			int winWidth, winHeight;
			glfwGetWindowSize(window, &winWidth, &winHeight);

			xpos = ( xpos - ( winWidth / 2 ) ) / (winWidth / 2);
			ypos = ( ypos - ( winHeight / 2 ) ) / ( -1 * winHeight / 2 );

			spline.addPoint(vec3(xpos, ypos, 0.0));
			spline.loadControlPoints();
			spline.loadBSpline();
			loadSpline();
		}

		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			int winWidth, winHeight;
			glfwGetWindowSize(window, &winWidth, &winHeight);

			xpos = (xpos - (winWidth / 2)) / (winWidth / 2);
			ypos = (ypos - (winHeight / 2)) / (-1 * winHeight / 2);

			spline.deletePoint(vec3(xpos, ypos, 0.0));
			spline.loadControlPoints();
			spline.loadBSpline();
			loadSpline();
		}

		else if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
				left_mouse_down = true;

			else if (action == GLFW_RELEASE)
				left_mouse_down = false;
		}
	}
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (!editMode){
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = (float) xpos - (float) lastX;
		float yoffset = (float) lastY -  (float) ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera.yaw += xoffset;
		camera.pitch += yoffset;

		if (camera.pitch > 89.0f)
			camera.pitch = 89.0f;
		if (camera.pitch < -89.0)
			camera.pitch = -89.0f;

		vec3 front;
		front.x = cos(radians(camera.yaw)) * cos(radians(camera.pitch));
		front.y = sin(radians(camera.pitch));
		front.z = sin(radians(camera.yaw)) * cos(radians(camera.pitch));
		camera.cameraFront = normalize(front);
		updateCamera();
	}
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void render()
{
	if (editMode)
	{
		glUseProgram(splineProgram);
	
		glBindVertexArray(splineVAO);
		glDrawArrays(GL_LINE_STRIP, 0, num_spline);
	
		glBindVertexArray(controlVAO);
		glDrawArrays(GL_LINES, 0, num_control);
	}
	else{
		glUseProgram(program);
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, num_points);
	}
}

/*
//This assumes ground is at z = 0
void trace_lightning_recursion(vec3 init_point, vec3 init_direction, vector<vec3> *storage, vec3 original)
{
vec3 rand_segment = vec3(0,0,0);
vec3 current_point = vec3(0,0,0);
double rand_angle = 0.0f;
double angle;
storage->push_back(init_point);

while(current_point.z > 0 && Random end check)
{
//START: Random angle, random vector

//END: Random angle, random vector

//This
if(/*Random check to branch)
{
trace_lightning(current_point, new direction, storage);
}
}
*/
