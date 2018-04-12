#pragma once

#include <vector>
#include "glm/ext.hpp"


struct Segment{
	glm::vec3 p0;
	glm::vec3 p1;
};

class lightning{
  public:

static float meter;


static float random_50_50();


static glm:: mat4 rotateAbout(glm::vec3 axis, float radians);

//This assumes ground is at z = 0
static void trace_lightning_recursion(glm::vec3 init_point, glm::vec3 init_direction, std::vector<Segment> *storage, int num_segs, float max_h, int recursion_depth);


static float uni_distribution(float min, float max, unsigned seed);

static  void trace_lightning(glm::vec3 init_point, glm::vec3 init_direction, std::vector<Segment> *storage, float max_h);

};
