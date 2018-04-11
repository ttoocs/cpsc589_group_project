#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
//#include <GLFW/glfw3.h>
#include "include/glm/ext.hpp"
#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <string>
#include <fstream>
#include <iterator>
#include <sstream>

	#define vPrint(X)   "(" << X.x << "," <<  X.y << "," << X.z << ")"

#define GL_SHADER_STORAGE_BUFFER 0x90D2

using namespace std;
using namespace glm;

int screenWidth = 1000;
int screenHeight = 1000;

unsigned int VBO;
unsigned int VAO;
int num_points;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int program;

GLuint segBuffer;



float meter = 0.004;

vector<vec3> storage;

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

struct Segment{
	vec3 p0;
	vec3 p1;
};

bool firstMouse = true;
float lastX = (float) screenWidth / 2.0;
float lastY = (float) screenHeight / 2.0;

vector<Segment> lightning_segs;
vector<float> vertices;

void genVBOsVAOs();
string LoadSource(const string &filename);
void initShaders();
void initPrograms();
void updateCamera();
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void loadPoints();
void render();

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Learn OpenGL", NULL, NULL);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	genVBOsVAOs();
	initShaders();
	initPrograms();
	updateCamera();

	loadPoints();


	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);

		//Render command
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		render();
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

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glGenBuffers(1, &segBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,segBuffer);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points
//		glEnableVertexAttribArray(0);

}

string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

void initShaders()
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
        vShaderFile.open("vertex_shader.glsl");
        fShaderFile.open("scene1FragShader.glsl");
        
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
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
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
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
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
		cout << "ERROR::SHADER::FRAGMENT::POINT::COMPILATION_FAILED\n" << infoLog << endl;
		exit(0);
	}
}

void initPrograms()
{
	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	int success;
	char infoLog[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::POINT::LINKING_FAILED\n" << infoLog << endl;
		exit(0);
	}
}

void updateCamera()
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 mvp;

	model = mat4(1.0f);
	view = camera.view();
	projection = perspective(radians(45.0f), (float) screenWidth / screenHeight, 0.1f, 100.0f);

	mvp = projection * view * model;

	glUseProgram(program);

	int mvpLoc = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));

	int cameraPosLoc = glGetUniformLocation(program, "cameraPosition");
	glUniform3f(cameraPosLoc, camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	/* CAMERA */

	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
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
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		loadPoints();

}

/*
	MOUSE PROCESSING FOR CAMERA TAKEN FROM: learnopengl.com
*/

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
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

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

mat4 rotateAbout(vec3 axis, float radians)
{
	mat4 matrix;

	matrix[0][0] = cos(radians) + axis.x*axis.x*(1 - cos(radians));
	matrix[1][0] = axis.x*axis.y*(1 - cos(radians)) - axis.z*sin(radians);
	matrix[2][0] = axis.x*axis.z*(1 - cos(radians)) + axis.y*sin(radians);

	matrix[0][1] = axis.y*axis.x*(1 - cos(radians)) + axis.z*sin(radians);
	matrix[1][1] = cos(radians) + axis.y*axis.y*(1 - cos(radians));
	matrix[2][1] = axis.y*axis.z*(1 - cos(radians)) - axis.x*sin(radians);

	matrix[0][2] = axis.z*axis.x*(1 - cos(radians)) - axis.y*sin(radians);
	matrix[1][2] = axis.z*axis.y*(1 - cos(radians)) + axis.x*sin(radians);
	matrix[2][2] = cos(radians) + axis.z*axis.z*(1 - cos(radians));

	return matrix;
}

float random_50_50()
{
	default_random_engine uni_gen;
	uniform_real_distribution<double> uni_distribution_50_50(0.0, 1.0);

	if (uni_distribution_50_50(uni_gen) >= 0.5)
		return 1.0;
	else
		return -1.0;
}

//This assumes ground is at z = 0
void trace_lightning_recursion(vec3 init_point, vec3 init_direction, vector<Segment> *storage, int num_segs, float max_h, int recursion_depth)
{
	default_random_engine norm_gen, uni_gen;
	normal_distribution<double> norm_distribution(45.0, 20.0);
	uniform_real_distribution<double> uni_distribution_length(meter, 10.0*meter);
	uniform_real_distribution<double> uni_distribution_branch(0.0, 1.0);
	uniform_real_distribution<double> uni_distribution_new_dir(0.0, 60.0);
	uniform_int_distribution<int> uni_distribution_segs(10, 150);

	vec3 rand_segment = vec3(0, 0, 0);
	vec3 current_point = init_point;
	double rand_angle = 0.0f;
	double angle;

	vec3 T = normalize(init_direction);
	vec3 N = normalize(cross(init_direction, vec3(0.0, 1.0, 0.0)));
	vec3 B = normalize(cross(T, N));

	while ((current_point.y > 0.0) && (num_segs > 0))
	{
		Segment s;
		s.p0 = current_point;
		//START: Random angles, get rand segment
		rand_angle = norm_distribution(norm_gen);
		rand_segment = rotateAbout(T, radians(random_50_50() * rand_angle)) * rotateAbout(N, radians(random_50_50() * rand_angle)) * vec4(init_direction, 0.0);
		rand_segment = uni_distribution_length(uni_gen) * rand_segment;

		//END: Random angles, get rand segment
		//Here, we should have the new segment direction and length

		s.p1 = current_point + rand_segment;
		current_point = s.p1; //current_point + rand_segment;
		
		storage->push_back(s);
		
		if (uni_distribution_branch(uni_gen) <= ((0.1*pow(10, -(current_point.y/max_h))) / 100.0*recursion_depth))
		{
			vec3 new_dir = rotateAbout(T, radians(random_50_50() * uni_distribution_new_dir(uni_gen)))
							* rotateAbout(N, radians(random_50_50() * uni_distribution_new_dir(uni_gen)))
							* rotateAbout(B, radians(random_50_50() * uni_distribution_new_dir(uni_gen)))
							* vec4(rand_segment, 0.0);
			trace_lightning_recursion(current_point, normalize(new_dir), storage, uni_distribution_segs(uni_gen), current_point.y, recursion_depth + 1);
		}
		num_segs--;
	}
}


float uni_distribution(float min, float max, unsigned seed)
{
	default_random_engine uni_gen(seed);
	uniform_real_distribution<double> uni_distribution(min, max);

	return uni_distribution(uni_gen);
}

void trace_lightning(vec3 init_point, vec3 init_direction, vector<Segment> *storage, float max_h)
{
	unsigned seed = glfwGetTime();
	default_random_engine norm_gen(seed), uni_gen(seed);
	normal_distribution<double> norm_distribution(45.0, 20.0);
	uniform_real_distribution<double> uni_distribution_length(meter, 10.0*meter);
	uniform_real_distribution<double> uni_distribution_branch(0.0, 1.0);
	uniform_real_distribution<double> uni_distribution_new_dir(0.0, 45.0);
	uniform_int_distribution<int> uni_distribution_segs(10, 150);

	vec3 rand_segment = vec3(0, 0, 0);
	vec3 current_point = init_point;
	double rand_angle = 0.0f;
	double angle;

	vec3 T = normalize(init_direction);
	vec3 N = normalize(cross(init_direction, vec3(0.0, 1.0, 0.0)));
	vec3 B = normalize(cross(T, N));
	
	
	
	while (current_point.y > 0.0)
	{
		Segment s;
		s.p0 = current_point;
		//START: Random angles, get rand segment
		rand_angle = norm_distribution(norm_gen);
		rand_segment = rotateAbout(T, radians(random_50_50() * rand_angle)) * rotateAbout(N, radians(random_50_50() * rand_angle)) * vec4(init_direction, 0.0);
		rand_segment = uni_distribution_length(uni_gen) * normalize(rand_segment);

		//END: Random angles, get rand segment
		//Here, we should have the new segment direction and length
		
		s.p1 = current_point + rand_segment;
		current_point = s.p1; //current_point + rand_segment;
		
		storage->push_back(s);
		
		//std::cout << "s:\t" << vPrint(s.p0) << "\t" << vPrint(s.p1) << std::endl;

		if (uni_distribution_branch(uni_gen) <= (0.1*pow(10, -(current_point.y/max_h))))
		{
			vec3 new_dir = rotateAbout(T, radians(random_50_50() * uni_distribution_new_dir(uni_gen))) * rotateAbout(N, radians(random_50_50() * uni_distribution_new_dir(uni_gen))) * vec4(rand_segment, 0.0);
			trace_lightning_recursion(current_point, normalize(new_dir), storage, uni_distribution_segs(uni_gen), current_point.y, init_point.y);
		}
	}
}

void loadPoints()
{
	lightning_segs.clear();
	trace_lightning(vec3(0.0, 2.0, 0.0), vec3(-0.5, -1.0, -0.5), &lightning_segs, 2.0);

	storage.push_back(vec3(-1.0, 1.0, 0.0));
	storage.push_back(vec3(1.0, 1.0, 0.0));
	storage.push_back(vec3(1.0, -1.0, 0.0));

	storage.push_back(vec3(1.0, -1.0, 0.0));
	storage.push_back(vec3(-1.0, -1.0, 0.0));
	storage.push_back(vec3(-1.0, 1.0, 0.0));

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, storage.size() * sizeof(vec3), storage.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	num_points = storage.size();

	GLfloat temp[((lightning_segs.size() * 2 * 4) + 4)];
	for (int i = 0; i < lightning_segs.size(); i++)
	{
		temp[i * 8 + 4] = lightning_segs[i].p0.x;
		temp[i * 8 + 4 + 1] = -lightning_segs[i].p0.y;
		temp[i * 8 + 4 + 2] = lightning_segs[i].p0.z;
		temp[i * 8 + 4 + 3] = 0;
		
		temp[i * 8 + 4 + 4] = lightning_segs[i].p1.x;
		temp[i * 8 + 4 + 5] = -lightning_segs[i].p1.y;
		temp[i * 8 + 4 + 6] = lightning_segs[i].p1.z;
		temp[i * 8 + 4 + 7] = 0;
	}


	//for (int i = 2; i < lightning_segs.size(); i++){
//		if( lightning_segs[i] != lightning_segs[i-2] || lightning_segs[i] != lightning_segs[i-1] )
//			std::cout << "Diff:\t" <<  vPrint(lightning_segs[i]) << "\t" << vPrint(lightning_segs[i-2]) << std::endl;
		//cout << vPrint(lightning_segs[i].p0) << "\t" << vPrint(lightning_segs[i].p1) << "\n" << endl;
	//}

    temp[0] = lightning_segs.size() * 2;
  /*
	int lightningLoc = glGetUniformLocation(program, "lightning_segs");
	glUniform3fv(lightningLoc, lightning_segs.size(), temp);

	int numSegsLoc = glGetUniformLocation(program, "numSegs");
	glUniform1i(numSegsLoc, lightning_segs.size());

  */
	//cout << lightning_segs.size() << endl;
 /* //Working example via testing colors:
int OFF=4;
temp[OFF+0] = 0;
temp[OFF+1] = 1.0;
temp[OFF+2] = -2.0;

temp[OFF+3] = 0; //

temp[OFF+4] = -1.0;
temp[OFF+5] = 0;
temp[OFF+6] = -2;

temp[OFF+7] = 1; //??

temp[OFF+8] = -1;
temp[OFF+9] = 0;
temp[OFF+10] = -2;

temp[OFF+11] = 0;// ??

temp[OFF+12] = 0;
temp[OFF+13] = -1;
temp[OFF+14] = -2;
// */
  //EX: a single sphere:
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, segBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(temp),&temp,GL_DYNAMIC_COPY);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
	int numSegsLoc = glGetUniformLocation(program, "numSegs");
	glUniform1i(numSegsLoc, lightning_segs.size());

}

void render()
{
	glUseProgram(program);


	// Draw the pointMasses
	glBindVertexArray(VAO);
	//glDrawArrays(GL_LINES, 0, num_points);
	glDrawArrays(GL_TRIANGLES, 0, num_points);
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
