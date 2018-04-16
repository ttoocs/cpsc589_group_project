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

	void process_placed_metaballs();
    void process_cloud_paper(Tris& t, int rounds);
	void process_cloud_paper_pos_rad(Tris& t, int rounds,std::vector<vec3> *positions,std::vector<float> *rads,bool down,bool up);
    static void create_cloud(std::vector<vec3> *verts, std::vector<GLuint> *idx,std::vector<vec3> *norms, int numOfClouds, int m_in_cloud, int rounds);


    cloud(float(*f)(vec3, vec3, float) = NULL , vec3 * pos=NULL, int initBalls=1, int rounds=2, int rad=2.0, float gap = 3.0,int type=0);
    cloud(std::vector<vec3>  pos,std::vector<float>  rad);

  static std::vector<cloud*> allClouds;
  static Tris getAllTris();

  Tris getTris();

  MBS getMBs();
  
  static MBS getAllMBs();

};
