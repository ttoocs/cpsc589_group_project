// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

in vec3 FragNormal;
in vec3 FragPos;

uniform vec3 d;
uniform int scene;


float vlen(vec3 a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

void main(void)
{
	FragmentColour = vec4(FragNormal, 1.0);
		
	if(scene ==5)
	{
		
		
		vec3 nor = FragNormal/vlen(FragNormal);
		vec3 light = normalize(vec3(0.f,0.f,-70.f)-FragPos);
		vec3 color = vec3(0.25f,0.9f,0.25f);
		vec3 specColor = vec3(0.25f,0.9f,0.25f);
		vec3 dir = (d - FragPos)/(vlen(d-FragPos));
	//Start:Final Heuristic Shading Equation
		FragmentColour = vec4(color*(vec3(.3,.3,.3) + vec3(0.5,0.5,0.5)*max(0,dot(nor,light)))+ 
				vec3(0.4,0.4,0.4)*specColor*pow(max(0,dot( dir,nor)),128),1.0);
	//End:Final Heuristic Shading Equation
	
	}
}
