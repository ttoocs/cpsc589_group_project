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

uniform bool RayTrace = false;

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
  if(RayTrace == false){

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
  FragmentColour = vec4(0.0, 0.0, 0.0, 0.0);
  vec4 cloudColor = main_c();
  vec4 lightningColor = main_l();
  
  FragmentColour += cloudColor;
  FragmentColour += lightningColor;
}
