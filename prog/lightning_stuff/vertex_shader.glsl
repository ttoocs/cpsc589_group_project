
#version 330 core

layout (location = 0) in vec3 aPos;

out vec2 vp;

uniform mat4 mvp;

void main()
{
	vp = aPos.xy;
	gl_Position = mvp * vec4(aPos, 1.0);
}
