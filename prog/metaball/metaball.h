//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

//Scott Saunders
// Moved the .cpp stuff out of the .h stuff.
// Moved the meta-ball functions out of the main and into metaball .h/.cpp
#pragma once

#include "../types.h"
#include "../marching_cube/reMarch.h"
#include <vector>


float WyvillMetaBall(vec3 mbpos,vec3 tpos, float radius);
float fanceyMB(vec3 mbpos,vec3 tpos, float radius);
float sphereMB(vec3 mbpos, vec3 tpos, float radius);

float inclass(vec3 mbpos, vec3 tpos, float radius);

class MetaBall
{
public:
  vec3 pos;
  float radius = -1;
	//Must create functions to pass in to here.
  float (*m_surfaceFunction)(vec3, vec3, float);
  int id = -1;
	
//  virtual float function(vec3 v) = 0;
//  virtual float function(double x, double y, double z) {return function(vec3(x,y,z));}
  MetaBall(vec3 newPos, double radius, float(*f)(vec3, vec3, float)=WyvillMetaBall); //Mbpos, TestPos, Rad
	
  float valueAt(vec3 loc);


  //Static Stuff!
  static std::vector<MetaBall*> metaballs;
  static std::vector<MetaBall*> *accumData; //State-like data for accumtMetaBallFuncs..
  static float accumMetaBallFuncs(vec3 point);
  static double accumMetaBallFuncs(double x, double y, double z) {return accumMetaBallFuncs(vec3(x,y,z));}
  static void March(std::vector<vec3> * verts, std::vector<GLuint> * idx, std::vector<vec3> * norms=NULL, std::vector<MetaBall*> * mbs=NULL, vec3 * lbound=NULL, vec3 * ubound=NULL, double granularity=0.125);

  static void setAccumData(std::vector<MetaBall*> * mbs=NULL);
};


// void loadPoints();
