#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\ext.hpp>
#include <iostream>
#include <vector>
#include <ctime>

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

/*
	CAMERA CLASS TAKEN FROM: learnopengl.com
*/

enum BallType {DEFAULT = 0, MURAKAMI = 1, NISHIMURA = 2, WYVILL = 3};

class MetaBall
{
public:
	vec3 pos;
	virtual float function(vec3 v) = 0;
};

class DefaultMetaBall : public MetaBall
{
public:
	DefaultMetaBall(vec3 newPos)
	{
		pos = newPos;
	}

	float function(vec3 v) 
	{
		return (1.0 / (pow((v.x - pos.x), 2.0) + pow((v.y - pos.y), 2.0) + pow((v.z - pos.z), 2.0)));
	}
};

class MurakamiMetaBall : public MetaBall
{
public:
	float radius;

	MurakamiMetaBall(vec3 newPos, float r) {
		pos = newPos;
		radius = r;
	}

	float function(vec3 v) 
	{
		float r = length(pos - v);
		if (r > radius)
			return pow((1.0 - pow((r / radius), 2.0)), 2.0);
		else
			return 0.0;
	}
};

class NishimuraMetaBall : public MetaBall
{
public:
	float radius;

	NishimuraMetaBall(vec3 newPos, float r)
	{
		pos = newPos;
		radius = r;
	}

	float function(vec3 v)
	{
		float knotVal = radius / 3.0;
		float r = length(pos - v);

		if (r <= knotVal)
		{
			return (1.0 - 3 * pow((r / radius), 2.0));
		}
		else if (r <= radius)
		{
			return (3.0 / 2.0) * (1 - pow((r / radius), 2.0));
		}
		else
			return 0.0;
	}
};

class WyvillMetaBall : public MetaBall
{
public:
	float radius;

	WyvillMetaBall(vec3 newPos, float r)
	{
		pos = newPos;
		radius = r;
	}

	float function(vec3 v)
	{
		float r = length(pos - v);

		if (r > radius)
			return 0;

		float term1, term2, term3;
		float R = r / radius;

		term1 = (-4.0 / 9.0) * pow(R, 6.0);
		term2 = (17.0 / 9.0) * pow(R, 4.0);
		term3 = (22.0 / 9.0) * pow(R, 2.0);
		float total = term1 + term2 - term3;
		//float total = (float) term1 + (float) term2 - (float) term3 + (float) 1.0;
		return term1 + term2 - term3 + 1.0;
	}
};

struct Camera
{
	float cameraSpeed = 0.05f;

	float yaw = 270.0f;
	float pitch = 0.0f;

	vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
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

// General vector to load vertices to VBO's
vector<float> vertices;
vector<DefaultMetaBall> defaultMBs;
vector<MurakamiMetaBall> murakamiMBs;
vector<NishimuraMetaBall> nishiMBs;
vector<WyvillMetaBall> wyvillMBs;

float threshold = 0.01;
BallType type = WYVILL;

void genVBOsVAOs();
void initShaders();
void initPrograms();
void updateCamera();
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void createDefaultMetaBall(vec3 pos);
void createMurakamiMetaBall(vec3 pos, float r);
void createNishimuraMetaBall(vec3 pos, float radius);
void createWyvillMetaBall(vec3 pos, float r);
void loadDefaultMetaBalls();
void loadMurakamiMetaBalls();
void loadNishimuraMetaBalls();
void loadWyvillMetaBalls();
float accumMetaBallFuncs(vec3 point);
void loadPoints();
void render();

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 mvp;\n"
"void main()\n"
"{\n"
"	gl_Position = mvp * vec4(aPos, 1.0);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.855f, 0.647f, 0.125f, 1.0f);\n"
"}\n\0";

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

	switch (type)
	{
	case DEFAULT:
		loadDefaultMetaBalls();
		break;

	case MURAKAMI:
		loadMurakamiMetaBalls();
		break;
	
	case NISHIMURA:
		loadNishimuraMetaBalls();
		break;

	case WYVILL:
		loadWyvillMetaBalls();
		break;
	}

	while (!glfwWindowShouldClose(window))
	{
		loadPoints();

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
}

void initShaders()
{
	// Vertex Shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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

	else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		//if (threshold > 0.005)
			threshold -= 0.005;
	}

	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		threshold += 0.005;
	}
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

void createDefaultMetaBall(vec3 pos)
{
	defaultMBs.push_back(DefaultMetaBall(pos));
}

void createMurakamiMetaBall(vec3 pos, float r)
{
	murakamiMBs.push_back(MurakamiMetaBall(pos, r));
}

void createNishimuraMetaBall(vec3 pos, float radius)
{
	nishiMBs.push_back(NishimuraMetaBall(pos, radius));
}

void createWyvillMetaBall(vec3 pos, float r)
{
	wyvillMBs.push_back(WyvillMetaBall(pos, r));
}

void loadDefaultMetaBalls()
{
	defaultMBs.clear();
	createDefaultMetaBall(vec3(-0.5, 0.5, 0.5));
	createDefaultMetaBall(vec3(0.5, -0.5, -0.5));
}

void loadMurakamiMetaBalls()
{
	murakamiMBs.clear();
	createMurakamiMetaBall(vec3(-2.0, 2.0, 2.0), 1.0);
	createMurakamiMetaBall(vec3(2.0, -2.0, -2.0), 1.0);
}

void loadNishimuraMetaBalls()
{
	nishiMBs.clear();
	createNishimuraMetaBall(vec3(1.0, 1.0, 1.0), 1.5);
	createNishimuraMetaBall(vec3(-0.5, 0.5, 0.5), 1.0);
	createNishimuraMetaBall(vec3(0.5, -0.5, -0.5), 1.0);
}

void loadWyvillMetaBalls()
{
	wyvillMBs.clear();
	createWyvillMetaBall(vec3(-0.5, 0.5, 0.5), 1.0);
	createWyvillMetaBall(vec3(0.5, -0.5, -0.5), 1.0);
}

float accumMetaBallFuncs(vec3 point)
{
	float accum = 0.0;

	switch (type)
	{

	case DEFAULT:
		for (int i = 0; i < defaultMBs.size(); i++)
		{
			accum += defaultMBs[i].function(point);
		}
		break;

	case MURAKAMI:
		for (int i = 0; i < murakamiMBs.size(); i++)
		{
			accum += murakamiMBs[i].function(point);
		}
		break;

	case NISHIMURA:
		for (int i = 0; i < nishiMBs.size(); i++)
		{
			accum += nishiMBs[i].function(point);
		}
		break;

	case WYVILL:
		for (int i = 0; i < wyvillMBs.size(); i++)
		{
			accum += wyvillMBs[i].function(point);
		}
		break;
	}
	return accum;
}


void loadPoints()
{
	float accumulator = 0.0;

	for (float z = -2.0; z < 2.0; z += 0.05)
	{
		for (float y = -2.0; y < 2.0; y += 0.05)
		{
			for (float x = -2.0; x < 2.0; x += 0.05)
			{
				accumulator = accumMetaBallFuncs(vec3(x, y, z));
				
				if (accumulator >= threshold)
				{
					vertices.push_back(x);
					if (y < 0.0)
						vertices.push_back(0.0);
					else
						vertices.push_back(y);
					vertices.push_back(z);
				}
				accumulator = 0.0;
			}
		}
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	num_points = vertices.size() / 3;

	// Clear vertices vector
	vertices.clear();
}

void render()
{
	glUseProgram(program);

	// Draw the pointMasses
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, num_points);
}

