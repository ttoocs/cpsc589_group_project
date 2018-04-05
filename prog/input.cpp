//Scott Saunders
//Simple input wrapepr


#include <math.h>
#include "input.h"
#include "camera.h"
#define TRNL 3.f
vec2 mousePos = vec2(0,0);
bool right = false;
bool left = false;


#include <iostream>

extern Camera activeCamera;


namespace input{

void printVec(vec2 v)
{
  std::cout <<"aVec: (x,y,z) = (" << v.x << ", " << v.y << ")\n";
}

void setup(GLFWwindow * window){
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //std::cout << activeCamera.pos.x << std::endl;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_A)
	{
		activeCamera.translate(TRNL*activeCamera.rotateAbout(activeCamera.up,PI/2.f)*(vec4(activeCamera.dir,0))/10.f);
	}
	else if (key == GLFW_KEY_D)
	{
		activeCamera.translate(-TRNL*activeCamera.rotateAbout(activeCamera.up,PI/2.f)*(vec4(activeCamera.dir,0))/10.f);
	}
	else if (key == GLFW_KEY_W)
	{
		activeCamera.translate(TRNL*(activeCamera.dir)/10.f);
	}
	else if (key == GLFW_KEY_S)
	{
		activeCamera.translate(-TRNL*(activeCamera.dir)/10.f);
	}
	else if (key == GLFW_KEY_R)
	{
		activeCamera.translate(TRNL*(activeCamera.up)/10.f);
	}
	else if (key == GLFW_KEY_F)
	{
		activeCamera.translate(-TRNL*(activeCamera.up)/10.f);
	}
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
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);
	vec2 diff = newPos - mousePos;
  if(right)
  {
    /*
    if(diff.x < 0){
      activeCamera.trackballRight(-acos(diff.x/diff.length())/100.0);
    }else{
      activeCamera.trackballRight(acos(diff.x/diff.length())/100.0);
    }
    
    if(diff.y < 0){
      activeCamera.trackballUp(-acos(diff.y/diff.length())/100.0);
    }else{
      activeCamera.trackballUp(acos(diff.y/diff.length())/100.0);
    }
    */
    
    activeCamera.trackballRight(-diff.x);
   // activeCamera.trackballUp(-diff.y);
  }
  else if(left)
  {
    activeCamera.trackballUp(-diff.y);
  }
  mousePos = newPos;
  //printVec(mousePos);
  //vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);

//  vec2 diff = newPos - mousePos;
/*
  if(mousePressed)
    cam.rotateCamera(-diff.x, diff.y);
*/
 // mousePos = newPos;
}

}
