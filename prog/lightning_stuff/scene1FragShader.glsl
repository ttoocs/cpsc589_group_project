// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================


//  Solo-running setup
//  #version 430          //As this _ALWAYS_ has to be frist, it is handled elsewhere.
#ifndef LIGHT_SUBSHADER
  #define SPHERE 1
  #define TRIANGLE 2
  #define PLANE 3

  #define IDENTITY4 mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)

  uniform mat4 cameraMatrix=IDENTITY4;
  uniform mat4 perspectiveMatrix=IDENTITY4;
  uniform mat4 modelviewMatrix = IDENTITY4;
  uniform mat4 windowRatio = IDENTITY4;

  // first output is mapped to the framebuffer's colour index by default
  out vec4 FragmentColour;
#else
  //Any Sub-Shader mode setup:

#endif

  uniform float FOV=90;

  #extension GL_NV_shader_buffer_load : enable
  // interpolated colour received from vertex stage
  in vec2 vp;


uniform mat4 mvp=IDENTITY4;

struct Segment
{
	vec3 p0;
	vec3 p1;
};

struct Ray
{
	vec3 origin;
	vec3 dir;
};

uniform vec3 cameraPosition;

#ifdef LIGHT_SUBSHADER
layout(std140, binding = 1) buffer ffs{
#else
layout(std140, binding = 0) buffer ffs{
#endif
  vec4 NumSegsVec;   //See define below for actual useage
  Segment[] Segs;
};

//Make NumSegs variable exist
#define NumSegs NumSegsVec.x 

float calcShortestVector(Ray r, Segment s, bool cylinder)

{
       vec3 u = r.dir;
       vec3 v = s.p1 - s.p0;
       vec3 w = r.origin - s.p0;

       float a = dot(u, u);
       float b = dot(u, v);
       float c = dot(v, v);
       float d = dot(u, w);
       float e = dot(v, w);

       float rc = (b*e - c * d) / (a*c - b * b);
       float tc = (a*e - b * d) / (a*c - b * b);

       vec3 point1, point2;
       point1 = r.origin + rc * u;

       if (cylinder)
       {
		   if (tc < 0)
               point2 = s.p0;
	       else if (tc > 1.0)
               point2 = s.p1;
           else
               point2 = s.p0 + tc * v;
	   }
	   else
	   {
		   if (tc < 0)
              return 100000;
           else if (tc > 1.0)
              return 100000;
           else
              point2 = s.p0 + tc * v;

		return length(point2 - point1);
	   }

		return length(point2 - point1);
}

vec3 calculateColor(float w, float n, Ray r)
{
	float dist;
	float max_r = float( 204.0/255.0);
	float max_g = float( 204.0/255.0);
	float max_b = float( 255.0/255.0);

	vec3 color = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < NumSegs; i++){
		dist = calcShortestVector(r, Segs[i], false);

		if (dist < w + 1.0){
			color.x = color.x + max_r*exp(-pow((dist / w),n));
			color.y = color.y + max_g*exp(-pow((dist / w),n));
			color.z = color.z + max_b*exp(-pow((dist / w),n));
		}
	}
	return color;
}

vec3 calculateGlow(float w, float l, Ray r)
{
	float dist;
	float max_r = 1;//float( 255.f/255.f);
	float max_g = 1;//float( 255.f/255.f);
	float max_b = 1;//float( 255.f/255.f);

	vec3 color = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < NumSegs; i++){
		dist = calcShortestVector(r, Segs[i], true);

		if (dist < w + l){
			color.x = color.x + max_r * l * exp(-pow((dist / w),2.0));
			color.y = color.y + max_g * l * exp(-pow((dist / w),2.0));
			color.z = color.z + max_b * l * exp(-pow((dist / w),2.0));
		}
	}
	return color;
}
#ifndef LIGHT_SUBSHADER
void main(void)
#else
vec4 main_l()
#endif
{
	//vec3 cameraPosition = vec3(0,0,0);
	float z = -(1.f/tan(FOV/2.f));	// Calculating z coord

    vec3 directionVector = normalize(vec3(vp.x,	// x coord for direction vector
								 vp.y,	// y coord for direction vector
								 z));	// z coord for direction vector

	// Assume the camera position is at the origin. *CHANGE THIS LATER TO ACCOMODATE
	// DIFFERENT CAMERA ANGLES* (Use a Uniform)

  mat4 t = cameraMatrix*modelviewMatrix;
//  mat4 t = modelviewMatrix*cameraMatrix;

	Ray r;
	r.origin = vec3(0.0, 0.0, 1.65);
	r.dir = directionVector;

  //Transform ray for camera controls
  bool Transform=true;
  if(Transform)
  {
    t = inverse(t);
    //Make origin/ray temps
    vec4 ot = t*mvp*vec4(r.origin,1);
    vec4 rt = t*mvp*vec4(r.dir,0);
    r.origin = vec3(ot.x,ot.y,ot.z);
    r.dir = vec3(rt.x,rt.y,rt.z);
  }

// -------------MAIN CALCULATION------------------------
	float width_I = 0.002;
	float n = 0.7;

	float l = 0.008;
	float width_G = 0.008;

	vec3 color = vec3(1.0, 1.0, 1.0);


  //Togle paperRender/not via commenting/uncomenting below
  
  #define PaperRender
  
  #ifdef PaperRender
    // Papers render:
  	color = color * (calculateColor(width_I, n, r)
  		             + calculateGlow(width_G, l, r));
  #else

    //Simple render
    color = vec3(0,0,0);
    for(int i=0; i < NumSegs ; i++){
      float d;
      d = calcShortestVector(r,Segs[i], false);
      if ( d <0.001)
        color +=vec3(1);
    
    }
  
  #endif

  #ifndef LIGHT_SUBSHADER
	FragmentColour = vec4(color, 1);
  #else
	return vec4(color, 0);
  #endif
}

