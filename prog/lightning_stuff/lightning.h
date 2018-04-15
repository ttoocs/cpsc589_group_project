#pragma once

#include <vector>
#include "../types.h"
//#include "glm/ext.hpp"
//#include <GL/gl3w.h>
//#include <GLFW/glfw3.h>
//#include "../gl_helpers.h"
#include "bspline.h"

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#define MYGLUEMARK

struct Plane{
	vec3 point;
	vec3 normal;
};

struct Segment{
	glm::vec3 p0;
	glm::vec3 p1;
};

namespace lightning{

extern GLfloat meter;
extern GLuint segmentBuffer;

extern float random_50_50();


extern glm:: mat4 rotateAbout(glm::vec3 axis, float radians);

//This assumes ground is at z = 0
extern void trace_lightning_recursion(glm::vec3 init_point, glm::vec3 init_direction, std::vector<Segment> *storage, int num_segs, float max_h, int recursion_depth, Plane pl);


extern float uni_distribution(float min, float max, unsigned seed);

extern  void trace_lightning(std::vector<glm::vec3> targets, std::vector<Segment> *storage, float max_h);

//extern void loadPoints();
void loadPoints(BSpline spline);



};
