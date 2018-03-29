//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

//Scott Saunders
// Moved the .cpp stuff out of the .h stuff.

#pragma once

#include "../types.h"

class MetaBall
{
public:
	vec3 pos;
	float radius = -1;
	//Must create functions to pass in to here.
  float (*m_surfaceFunction)(vec3, float);
  int id = -1;
	
	virtual float function(vec3 v) = 0;
	MetaBall(vec3 newPos, double radius, float(*f)(vec3, float));
	
  float valueAt(vec3 loc);
};
