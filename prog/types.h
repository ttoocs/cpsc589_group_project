#pragma once


#define PI 3.1415926535897939
#define deVec(X) X.x,X.y,X.z

#include <vector>
#include <glm/glm.hpp>
//#include "glm/glm/glm.hpp"
#include "gl_helpers.h"

typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;


#define sVec(A) "(" << A.x << "," << A.y << "," << A.z << ")"


struct Tris {
  std::vector<vec3>   * verts =NULL;
  std::vector<vec3>   * norms =NULL;
  std::vector<GLuint> * idx   =NULL;
};
typedef struct Tris Tris;

inline Tris toTris ( std::vector<vec3>   * vertsi, std::vector<vec3>   * normsi, std::vector<GLuint> * idxi){
  Tris t;
  t.verts = vertsi;
  t.norms = normsi;
  t.idx = idxi;
  return t;
}

#define expandTris(X) X->verts,X->norms,X->idx


inline Tris mergeTris( Tris t1, Tris t2){
  Tris r;
  if(t1.verts ==NULL){
    r.verts = t2.verts;
    r.norms = t2.norms;
    r.idx = t2.idx;
    return r;
  }
  r.verts = t1.verts;
  r.norms = t1.norms;
  r.idx = t1.idx;

  
  int offset = t1.verts->size();
  for(int i=0; i < t2.verts->size() ; i++){
    bool found = false;
    for(int j=0; j < r.verts->size(); j++){
      if(((*r.verts)[j] == (*t2.verts)[i]) && ((*r.norms)[j] == (*t2.norms)[i])){
        (*r.idx).push_back(j+offset);
        found=true;
        break;
      }
      
    }

    if(!found){
      r.verts->push_back((*t2.verts)[i]);
      r.norms->push_back((*t2.norms)[i]);
      r.idx->push_back((*t2.idx)[i] + offset);
    
    }

  }

  return r;
}


/*
#include <Eigen/Dense>

typedef Eigen::Matrix<float, 4, 1> vec4;
typedef Eigen::Matrix<float, 3, 1> vec3;
typedef Eigen::Matrix<float, 2, 1> vec2;

typedef Eigen::Matrix<float, 4, 4> mat4;
typedef Eigen::Matrix<float, 3, 3> mat3;
typedef Eigen::Matrix<float, 2, 2> mat2;
*/

