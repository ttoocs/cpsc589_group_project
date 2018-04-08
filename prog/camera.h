#ifndef CAMERA_H
#define CAMERA_H


#include "types.h"
//#include "glm/glm.hpp"
#include <cstdio>

//using namespace glm;
class Camera
{
public:
	float cameraSpeed = 0.05f;

	float yaw = 270.0f;
	float pitch = 0.0f;

	vec3 cameraPos = vec3(0.0f, 0.0f, 1.0f);
	vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
	vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

	mat4 view();
};
#endif


