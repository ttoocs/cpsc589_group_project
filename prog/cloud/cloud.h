//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.

#pragma once

#include <vector>
#include "../metaball/metaball.h"
#include "../gl_helpers.h"

class cloud{
  public:
    std::vector<MetaBall*> balls;

    std::vector<vec3> verts;
    std::vector<vec3> norms;
    std::vector<GLuint> idx;

    void process_cloud_paper(std::vector<vec3> *points, std::vector<GLuint> *indices,std::vector<vec3> *norms, int rounds);
    void process_cloud_naive(std::vector<vec3> *points, std::vector<GLuint> *indices,std::vector<vec3> *norms, int rounds);
    static void create_cloud(std::vector<vec3> *verts, std::vector<GLuint> *idx,std::vector<vec3> *norms, int numOfClouds, int m_in_cloud, int rounds);


    cloud(float(*f)(vec3, vec3, float) = NULL , vec3 * pos=NULL, int initBalls=0, int rounds=10, int rad=1);

};
