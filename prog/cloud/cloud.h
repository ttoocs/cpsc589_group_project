//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.

#pragma once

#include <vector>
#include "../metaball/metaball.h"
#include "../gl_helpers.h"

class cloud{
  public:
     std::vector<MetaBall*> balls;

    Tris tris;

    void process_cloud_paper(Tris& t, int rounds);
    static void create_cloud(std::vector<vec3> *verts, std::vector<GLuint> *idx,std::vector<vec3> *norms, int numOfClouds, int m_in_cloud, int rounds);

    cloud(float(*f)(vec3, vec3, float) = NULL , vec3 * pos=NULL, int initBalls=0, int rounds=30, int rad=2.0,bool skip=false);

  static std::vector<cloud*> allClouds;
  static Tris getAllTris();

  Tris getTris();

  MBS getMBs();
  
  static MBS getAllMBs();

};
