// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 430
#define SPHERE 1
#define TRIANGLE 2
#define PLANE 3

#extension GL_NV_shader_buffer_load : enable
// interpolated colour received from vertex stage
in vec2 vp;
// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

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

layout(std140, binding = 0) buffer ffs{
  vec4 NumSegsVec;   //See define below for actual useage
  Segment[] Segs;
};

//Make NumSegs variable exist
#define NumSegs NumSegsVec.x 

float calcShortestVector(Ray r, Segment s)

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

       if (tc < 0)
              point2 = s.p0;
       else if (tc > 1.0)
              point2 = s.p1;
       else
              point2 = s.p0 + tc * v;

		return length(point2 - point1);
}

vec3 calculateColor(float w, float n, Ray r)
{
	float dist;
	float max_r = (float) 204.0/255.0;
	float max_g = (float) 255.0/255.0;
	float max_b = (float) 255.0/255.0;

	vec3 color = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < NumSegs; i++){
		dist = calcShortestVector(r, Segs[i]);

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
	float max_r = (float) 255.0/255.0;
	float max_g = (float) 255.0/255.0;
	float max_b = (float) 255.0/255.0;

	vec3 color = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < NumSegs; i++){
		dist = calcShortestVector(r, Segs[i]);

		if (dist < w + l){
			color.x = color.x + max_r * l * exp(-pow((dist / w),2.0));
			color.y = color.y + max_g * l * exp(-pow((dist / w),2.0));
			color.z = color.z + max_b * l * exp(-pow((dist / w),2.0));
		}
	}
	return color;
}

void main(void)
{
	//vec3 cameraPosition = vec3(0,0,0);
	float z = -(1.f/tan(90.0/2.f));	// Calculating z coord

    vec3 directionVector = normalize(vec3(vp.x,	// x coord for direction vector
								 vp.y,	// y coord for direction vector
								 z));	// z coord for direction vector

	// Assume the camera position is at the origin. *CHANGE THIS LATER TO ACCOMODATE
	// DIFFERENT CAMERA ANGLES* (Use a Uniform)

	Ray r;
	r.origin = cameraPosition;
	r.dir = directionVector;

// -------------MAIN CALCULATION------------------------
	float width_I = 0.005;
	float n = 0.5;

	float l = 0.001;
	float width_G = 0.001;

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
      d = calcShortestVector(r,Segs[i]);
      if ( d <0.01)
        color +=vec3(0.1);
    
    }
  
  #endif

	FragmentColour = vec4(color, 1);
}

