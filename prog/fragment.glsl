//Simple fragment shader


#version 430

// interpolated colour received from vertex stage

in vec2 FragUV;
in vec3 FragNormal;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform ivec2 dimentions;
uniform sampler2D image;

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
	vec4 colour;
	vec2 coord = FragUV;
	vec3 n_proj = proj(FragNormal,vec3(0,-1,0));
	vec3 white = vec3(1);


	coord.x = coord.x + 1.f;
	coord.y = coord.y + 1.f;

	coord.x /= 2;
	coord.y /= 2;

	
	if(n_proj.y > 0)
	{
		colour = vec4(white*((n_proj.y+1)/2),0);
	}
	else
	{
		colour = vec4(white*((n_proj.y+1)/2),0);
		//colour = vec4(0,0,0,0);
	}
	
	
	//colour = vec4(FragNormal,0);

    FragmentColour = colour;
}
