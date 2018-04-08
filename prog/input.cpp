//Scott Saunders
//Simple input wrapepr


#include <math.h>
#include "input.h"
#include "camera.h"
#define TRNL 3.f
bool firstMouse = true;
float lastX = 0;
float lastY = 0;
bool left = false;
bool right = false;


#include <iostream>

extern Camera activeCamera;
extern int nextRound;
extern mat4 winRatio;


namespace input{

void printVec(vec2 v)
{
  std::cout <<"aVec: (x,y,z) = (" << v.x << ", " << v.y << ")\n";
}

void setup(GLFWwindow * window){
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetWindowSizeCallback(window, resizeCallback);
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //std::cout << activeCamera.pos.x << std::endl;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if (key ==GLFW_KEY_W)
	{
		activeCamera.cameraPos += activeCamera.cameraSpeed * activeCamera.cameraFront;
	}
	if (key ==GLFW_KEY_S)
	{
		activeCamera.cameraPos -= activeCamera.cameraSpeed * activeCamera.cameraFront;
	}
	if (key ==GLFW_KEY_A)
	{
		activeCamera.cameraPos -= normalize(cross(activeCamera.cameraFront, activeCamera.cameraUp)) * activeCamera.cameraSpeed;
	}
	if (key ==GLFW_KEY_D)
	{
		activeCamera.cameraPos += normalize(cross(activeCamera.cameraFront, activeCamera.cameraUp)) * activeCamera.cameraSpeed;
	}/*
	else if (key == GLFW_KEY_R)
	{
		activeCamera.translate(TRNL*(activeCamera.up)/10.f);
	}
	else if (key == GLFW_KEY_F)
	{
		activeCamera.translate(-TRNL*(activeCamera.up)/10.f);
	}
	else if (key == GLFW_KEY_Z)
	{
		nextRound =1;
	}
  * */
}

bool mousePressed;
void mouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
{
  
  if( ((action == GLFW_PRESS) || (action == GLFW_RELEASE))&& key == GLFW_MOUSE_BUTTON_RIGHT )
    right = !right;
  else if( ((action == GLFW_PRESS) || (action == GLFW_RELEASE))&& key == GLFW_MOUSE_BUTTON_LEFT )
    left = !left;
  
}


vec2 mousePos;
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
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

  if(left)
  {
    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    activeCamera.yaw += xoffset;
    activeCamera.pitch += yoffset;

    if (activeCamera.pitch > 89.0f)
      activeCamera.pitch = 89.0f;
    if (activeCamera.pitch < -89.0)
      activeCamera.pitch = -89.0f;

    vec3 front;
    front.x = cos(glm::radians(activeCamera.yaw)) * cos(glm::radians(activeCamera.pitch));
    front.y = sin(glm::radians(activeCamera.pitch));
    front.z = sin(glm::radians(activeCamera.yaw)) * cos(glm::radians(activeCamera.pitch));
    activeCamera.cameraFront = normalize(front);
  }
}
void resizeCallback(GLFWwindow* window, int width, int height)
{
  
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glViewport(0, 0, width, height);

	float minDim = float(std::min(width, height));

	winRatio[0][0] = minDim/float(width);
	winRatio[1][1] = minDim/float(height);
  
}

}
