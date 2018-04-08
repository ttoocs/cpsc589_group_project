
#version 330 core

layout (location = 0) in vec3 aPos;

out vec2 vp;

void main()
{
	vp = aPos.xy;
	gl_Position = vec4(aPos, 1.0);
}
