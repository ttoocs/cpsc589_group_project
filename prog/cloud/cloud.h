//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.

#pragma once

#include <vector>
#include "../metaball/metaball.h"
#include "../gl_helpers.h"

class cloud{
  public:
    std::vector<MetaBall*> balls;
    void create_cloud(std::vector<vec3> *points, std::vector<GLuint> *indices,std::vector<vec3> *norms, int rounds);
};
