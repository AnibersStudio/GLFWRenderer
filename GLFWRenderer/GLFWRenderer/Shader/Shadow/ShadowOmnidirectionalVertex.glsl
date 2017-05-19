#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 WVP;

out vec3 fragpos;

void main()
{
	gl_Position  = WVP * vec4(position, 1.0f);
	fragpos = position;
}