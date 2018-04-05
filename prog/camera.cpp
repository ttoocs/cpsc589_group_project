//Original belonged to Jeremy Hartd
//Modified by Cory Jensen

#include "camera.h"
#include <iostream>

mat4 Camera::rotateAbout(vec3 axis, float radians)
{
	mat4 matrix;

	matrix[0][0] = cos(radians) + axis.x*axis.x*(1-cos(radians));
	matrix[1][0] = axis.x*axis.y*(1-cos(radians))-axis.z*sin(radians);
	matrix[2][0] = axis.x*axis.z*(1-cos(radians)) + axis.y*sin(radians);

	matrix[0][1] = axis.y*axis.x*(1-cos(radians)) + axis.z*sin(radians);
	matrix[1][1] = cos(radians) + axis.y*axis.y*(1-cos(radians));
	matrix[2][1] = axis.y*axis.z*(1-cos(radians)) - axis.x*sin(radians);

	matrix[0][2] = axis.z*axis.x*(1-cos(radians)) - axis.y*sin(radians);
	matrix[1][2] = axis.z*axis.y*(1-cos(radians)) + axis.x*sin(radians);
	matrix[2][2] = cos(radians) + axis.z*axis.z*(1-cos(radians));

	return matrix;
}

void Camera::trackballUp(float radians)
{
    //std::cout << "In trackUp Right dot up: " << dot(right,up) << std::endl;
	mat4 rotation = rotateAbout(right, -radians);

//	vec4 newPos = rotation*vec4(pos, 1);
//	pos = vec3(newPos.x, newPos.y, newPos.z);

	vec4 newUp = rotation*vec4(up, 1);
	up = normalize(vec3(newUp.x, newUp.y, newUp.z));

	dir = normalize(-cross(right, up));
    //std::cout << "In trackUp Right dot up: " << dot(right,up) << std::endl;
}

void printVec(vec4 v)
{
  std::cout <<"aVec: (x,y,z) = (" << v.x << ", " << v.y << ", " << v.z <<")\n";
}
void Camera::trackballRight(float radians)
{
    //std::cout << "In trackRight Right dot up: " << dot(right,up) << std::endl;
	mat4 rotation = rotateAbout(vec3(0,1,0), radians);

//	vec4 newPos = rotation*vec4(pos, 1);
//	pos = vec3(newPos.x, newPos.y, newPos.z);

	vec4 newRight = rotation*vec4(right, 1);
	right = normalize(vec3(newRight.x, newRight.y, newRight.z));
 // std::cout << "before rotation\n";
 //   printVec(vec4(up,0));
	up = rotation * vec4(up, 1);
//    printVec(vec4(up,0));

	dir = normalize(-cross(right, up));
//	up = normalize(-cross(right, dir));
  /*
    std::cout << "uplength: " << up.length << std::endl;
    printVec(vec4(up,0));
    std::cout << "rightlength: " << right.length << std::endl;
    printVec(vec4(right,0));
    std::cout << "dirlength: " << dir.length << std::endl;
    printVec(vec4(dir,0));
    std::cout << "In trackRight Right dot up: " << dot(right,up) << std::endl;
    * */
   // std::cout << "In trackRight Right dot up: " << dot(right,up) << std::endl;
}

void Camera::zoom(float factor)
{
	pos = -dir*length(pos)*factor;
}


void Camera::translate(vec3 toTrans)
{
	pos += toTrans;
}

mat4 Camera::getMatrix()
{
	mat4 cameraRotation = mat4(
			vec4(right, 0),
			vec4(up, 0),
			vec4(-dir, 0),
			vec4(0, 0, 0, 1));

	mat4 translation = mat4 (
			vec4(1, 0, 0, 0),
			vec4(0, 1, 0, 0),
			vec4(0, 0, 1, 0),
			vec4(-pos, 1));

	return transpose(cameraRotation)*translation;
}
