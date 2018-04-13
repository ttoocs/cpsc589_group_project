//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.

#pragma once

#include <vector>
#include "../metaball/metaball.h"
#include "../gl_helpers.h"

class cloud{
  public:
    std::vector<MetaBall*> balls;

//    std::vector<vec3> verts;
//    std::vector<vec3> norms;
//    std::vector<GLuint> idx;

    Tris tris;

//    typedef (*tris.verts) verts;
//    typedef (*tris.norms) norms;
//    typedef (*tris.idx)   idx;

    void process_cloud_naive(std::vector<vec3> *points, std::vector<GLuint> *indices,std::vector<vec3> *norms, int rounds);
    void process_cloud_paper(std::vector<vec3> *points, std::vector<GLuint> *indices,std::vector<vec3> *norms, int rounds){
//        std::cout << "Stubbed" << std::endl;
//        return;
      Tris t = toTris(points,norms,indices); 
      process_cloud_paper(t,rounds);
      points = t.verts; indices=t.idx; norms = t.norms;
      tris = t;
    }
    void process_cloud_paper(Tris& t, int rounds);
    static void create_cloud(std::vector<vec3> *verts, std::vector<GLuint> *idx,std::vector<vec3> *norms, int numOfClouds, int m_in_cloud, int rounds);


    cloud(float(*f)(vec3, vec3, float) = NULL , vec3 * pos=NULL, int initBalls=3, int rounds=5, int rad=2.0,bool skip=false);
//    cloud(); //Default constructor.. risky.


  static std::vector<cloud*> allClouds;
  static Tris getAllTris();

  Tris getTris();
};
