#version 430 core
layout (location = 0) in vec3 position;

out float zinview;

uniform mat4 WVP;

void main()
{
	vec4 fragpos = WVP * vec4(position, 1.0f);
	gl_Position = fragpos;
	zinview = fragpos.z;
}