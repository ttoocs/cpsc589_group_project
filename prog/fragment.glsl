//Simple fragment shader

#version 430

#define LIGHT_SUBSHADER //Tell Light fragment shader to be subordinate.

#define IDENTITY4 mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)

// interpolated colour received from vertex stage

in vec2 FragUV;
in vec3 FragNormal;

vec4 main_l();
vec4 main_c();

  //Camera Stuffs:
  uniform mat4 cameraMatrix=IDENTITY4;
  uniform mat4 perspectiveMatrix=IDENTITY4;
  uniform mat4 modelviewMatrix = IDENTITY4;
  uniform mat4 windowRatio = IDENTITY4;


uniform ivec3 RayTrace = ivec3(0,1,1);

#define pass1 RayTrace.y
#define pass2 RayTrace.z

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

//The projection of v0 onto v1
//returns zero vecor if v1 has length 0
vec3 proj(vec3 v0, vec3 v1)
{
	if(v1.length == 0)
		return vec3(0);
	//return dot(v0,v1)*v1/(v1.length*v1.length);
	return (dot(v0,v1)*v1) / (dot(v1,v1));
}

void main(void)
{
  if(RayTrace.x == 0){

	vec4 colour;
	vec2 coord = FragUV;
	vec3 n_proj = proj(FragNormal,vec3(0,-1,0));
	vec3 white = vec3(1);

	coord.x = coord.x + 1.f;
	coord.y = coord.y + 1.f;

	coord.x /= 2;
	coord.y /= 2;

//  #define rendType1  // Simple white->dark transition  
  #define rendType2  // N- color mixing based on a normal
//  #define rendType3  

  #ifdef rendType1
	if(n_proj.y > 0)
	{
		colour = vec4(white*((n_proj.y+1)/2),0);
	}
	else
	{
		colour = vec4(white*((n_proj.y+1)/2),0);
		//colour = vec4(0,0,0,0);
	}
	#endif
	
  #ifdef rendType2
  
  const int numColors = 3;
  vec3 norms[numColors];
  vec3 colors[numColors];
  vec3 ambient = vec3(0);
 
  //Note: the following are + / -
  //Note: X: R/L 
  //Note: Y: Up/down,
  //Note: Z: Into/Outof (screen)

  //Sunlite air:
    colors[0] = vec3(.8);  //White
    norms[0]  = vec3(0,1,0);   //Up
  
  //Under-side:
//    rgb(35, 41, 51)
    colors[1] = vec3(0.13,0.16,0.20);
    norms[1]  = vec3(0,-1,0);
  
  //left-sunglowyness
    colors[2] = vec3(.7,0.5,0.25);
    norms[2]  = vec3(-1,0.5,-0.2);

  colour = vec4(ambient,0);
  for(int i = 0; i < numColors; i++){
    float p = dot(FragNormal,norms[i]);
    if(p > 0)
      colour += vec4( p* colors[i], 0);
  }

  #endif
 
  #ifdef rendType3


  #endif
	
	//colour = vec4(FragNormal,0);

    FragmentColour = colour;
    return;
  }

  //RayTrace Code here:
//  FragmentColour = vec4(1,1,1,1); //main_c();

  vec4 c = vec4(0);
/*
  if(pass1 == 1)
    c = vec4(1,0,0,0);
  if(pass1 == 2)
    c = vec4(0,1,0,0);
  if(pass1 == 3)
    c = vec4(0,0,1,0);
  if(pass1 == 0)
    c = vec4(1);
  if(isnan(pass1))
    c = vec4(1,1,0,0);
  if(isinf(pass1))
    c = vec4(0,1,1,0);
*/
//  /*
  if( pass1 == 1 || pass1 == 3){
    c += main_c();
    if(c.x < 0) 
      c.x=0;
    if(c.y < 0) 
      c.y=0;
    if(c.z < 0) 
      c.z=0;
    if(c.w < 0) 
      c.w=0;
  }
  if( pass1  == 2 || pass1 == 3){
    c += main_l();
  }
//  */
  FragmentColour = c;
}
