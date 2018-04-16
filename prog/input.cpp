//Scott Saunders
//Simple input wrapepr


#include <math.h>
#include "input.h"
#include "camera.h"
#include "metaball/metaball.h"
#include "cloud/cloud.h"

#include "main.h"

#define TRNL 3.f
bool firstMouse = true;
float lastX = 0;
float lastY = 0;
bool Left = false;
bool right = false;


#include <iostream>

extern Camera activeCamera;
extern cloud aCloud;
extern Tris renderTris;
extern int havePoints;
extern mat4 winRatio;

extern GLFWwindow * window;
extern std::vector<vec3> positions;
extern std::vector<float> radiuss;
    std::vector<vec3> spare = positions;
    std::vector<float> spare2 = radiuss;
int meshMode = GL_LINE;
float radius = 1.0f;
bool up = true;
bool down = true;

extern float thres;
extern float pass1;
extern float pass2;

namespace input{

void printVec(vec2 v)
{
  std::cout <<"aVec: (x,y,z) = (" << v.x << ", " << v.y << ")\n";
}

void cloudHelp();

void setup(GLFWwindow * window){
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetWindowSizeCallback(window, resizeCallback);

  cloudHelp();
}


void cloudHelp(){
  std::cout << "[w/a/s/d/f/r]\t- Move camera about" << std::endl;
  std::cout << "[u]\t\t- Undo cloud add operation" << std::endl;
  std::cout << "[j]\t\t- Run cloud processing" << std::endl;
  std::cout << "[i/k]\t\t- Toggle Upward/Downward growth" << std::endl;
  std::cout << "[t/g]\t\t- Increase/decrease radius of new cloud." << std::endl;
  std::cout << "[m]\t\t- Toggle wireframe" << std::endl;
  std::cout << "RightClick\t- Place meta-ball at camera position" << std::endl;
  std::cout << "LeftClick\t- Drag to rotate camera." << std::endl;
  std::cout << "[y/h]\t\t- Increase/decrease thresholds of clouds." << std::endl;
}


void BsplineCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
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
//		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			//glBindVertexArray(VAO);
			//lightning::loadPoints(spline);
//		}
	}
	/* CAMERA */
/*
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
  */


void RayCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(action != GLFW_RELEASE){
	if (key ==GLFW_KEY_Y && action == GLFW_PRESS)
	{
    thres+= 0.2;
    std::cout << thres << std::endl;
	}
	if (key ==GLFW_KEY_H && action == GLFW_PRESS)
	{
    thres-= 0.2;
    std::cout << thres << std::endl;
	}

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
	}
	if (key ==GLFW_KEY_F)
	{
		activeCamera.cameraPos -= activeCamera.cameraUp * activeCamera.cameraSpeed;
	}
	if (key ==GLFW_KEY_R)
	{
		activeCamera.cameraPos += activeCamera.cameraUp * activeCamera.cameraSpeed;
	}

  }
}

void CloudCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if (action != GLFW_RELEASE){
	if (key ==GLFW_KEY_Y && action == GLFW_PRESS)
	{
    thres+= 0.2;
    std::cout << thres << std::endl;
	}
	if (key ==GLFW_KEY_H && action == GLFW_PRESS)
	{
    thres-= 0.2;
    std::cout << thres << std::endl;
	}
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
	}
	if (key ==GLFW_KEY_F)
	{
		activeCamera.cameraPos -= activeCamera.cameraUp * activeCamera.cameraSpeed;
	}
	if (key ==GLFW_KEY_R)
	{
		activeCamera.cameraPos += activeCamera.cameraUp * activeCamera.cameraSpeed;
	}
  }
	if (key ==GLFW_KEY_U && action == GLFW_PRESS)
	{
		if(positions.size() >1)
    {
      positions.pop_back();
      radiuss.pop_back();
      aCloud = *(new cloud(positions,radiuss));
    }
	}
	if (key ==GLFW_KEY_J && action == GLFW_PRESS)
	{
		aCloud.process_cloud_paper_pos_rad(aCloud.tris, 1,&spare,&spare2,up,down);
    aCloud = *(new cloud(spare,spare2));
	}
	if (key ==GLFW_KEY_I && action == GLFW_PRESS)
	{
		up = !up;
    if(!up)
    {
      std::cout << "Does not grow up.\n";
    }
    else
    {
      std::cout << "Grows up.\n";
    }
	}
	if (key ==GLFW_KEY_K && action == GLFW_PRESS)
	{
		down = !down;
    if(!down)
    {
      std::cout << "Does not grow down.\n";
    }
    else
    {
      std::cout << "Grows down.\n";
    }
	}
	if (key ==GLFW_KEY_T && action == GLFW_PRESS)
	{
		radius += 0.1;
    std::cout << "Radius size: " << radius << std::endl;
	}
	if (key ==GLFW_KEY_G && action == GLFW_PRESS)
	{
    radius -=0.1f;
    if(radius <= 0.1f)
      radius = 0.1f;
    std::cout << "Radius size: " << radius << std::endl;
	}
  if((key == GLFW_KEY_M)&&(action == GLFW_PRESS))
  {
    if(meshMode == GL_LINE)
    {
			glPolygonMode(GL_FRONT_AND_BACK, meshMode);
      meshMode = GL_FILL;
    }
    else
    {
			glPolygonMode(GL_FRONT_AND_BACK, meshMode);
      meshMode = GL_LINE;
    }
  }
} //End cloud call-back.

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
  if (key == GLFW_KEY_1 && action == GLFW_PRESS){
    std::cout << "Switching to Cloud Mode." << std::endl;
    MODE=MODE_CLOUD;
    //TODO
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS){
    std::cout << "Switching to Raytrace Mode." << std::endl;
    MODE=MODE_RAY;
    //TODO
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS){
    MODE=MODE_BSPLINE;
    std::cout << "Switching to BSpline edit Mode." << std::endl;
    //TODO
  }

  if (MODE == MODE_CLOUD || MODE == MODE_RAY){
    CloudCallback( window,  key, scancode, action, mods);
  }else if (MODE == MODE_RAY){
    RayCallback(window,  key, scancode, action, mods);
  }else if (MODE == MODE_BSPLINE){
    BsplineCallBack(window,  key, scancode, action, mods);
  }


}

bool mousePressed;
void mouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
{
 
  if(MODE == MODE_CLOUD || MODE == MODE_RAY){
 
  if( ((action == GLFW_PRESS))&& key == GLFW_MOUSE_BUTTON_RIGHT )
  {
    if(MODE == MODE_CLOUD){
      positions.push_back(activeCamera.cameraPos+3.0f*(activeCamera.cameraFront));
      radiuss.push_back(radius);
      spare = positions;
      spare2 = radiuss;
      aCloud = *(new cloud(positions,radiuss));
    }
  }
  else if( ((action == GLFW_PRESS) || (action == GLFW_RELEASE))&& key == GLFW_MOUSE_BUTTON_LEFT )
    Left = !Left;
  }

	if (MODE == MODE_BSPLINE)
	{
		if (key == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
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

		else if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
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

		else if (key == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
				Left = true;

			else if (action == GLFW_RELEASE)
				Left = false;

      if(Left){
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

      }
		}
	}
  


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

  if(Left)
  {
    xoffset *= activeCamera.lookSpeed;
    yoffset *= activeCamera.lookSpeed;

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
