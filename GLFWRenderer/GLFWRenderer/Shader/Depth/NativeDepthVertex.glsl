#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 WVP;
out float depth;

void main()
{
	vec4 fragpos = WVP * vec4(position, 1.0f);
	gl_Position = fragpos;
	depth = fragpos.w;
}