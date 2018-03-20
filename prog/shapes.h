#pragma once

#include "types.h"
#include <vector>

#define torad(X)  ((float)(X*PI/180.f))

#define V_PUSH(X,a,b,c) X.push_back(vec3(a,b,c)); //Re-wrttien for GLM.

#define P_indices(X,a,b,c)	      X.push_back(a); X.push_back(b); X.push_back(c);

struct TextureStuff{
	int components;	
	int tWidth;
	int tHeight;
	unsigned char* data;
};
//typedef struct TextureStuff;

typedef struct Object Object; 
struct Object{ 
  std::vector<vec3> positions; 
  std::vector<vec3> normals;     //What are these used for? -- Lighting. 
  std::vector<vec2> uvs;     //My guess this is for textures? 
  std::vector<unsigned int> indices; 
	TextureStuff texture;
	mat4 modelview;
};

void nukeshape(Object *obj);

void loadTexture(Object *obj, const char* filename);

void moveObj(Object *obj, vec3 pos);

void rotateObj(Object *obj,vec3 axis, float radians);

void rotateObjPos(Object *obj,vec3 axis, float radians);

void generateTri(Object *obj);

//u parameterizes in the big circle, v parameterizes in the little circle
//c_r is the circle radius, t_r is the tube radius
void generateTorus(Object * obj, float c_r, float t_r, int uDivisions, int vDivisions);

void generateSphere(Object * obj, float radius, int udiv, int vdiv);



