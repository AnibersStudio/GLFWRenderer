#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in mat4 transform;

uniform mat4 WVP;

void main()
{
	gl_Position = WVP * transform * vec4(position, 1.0f);
}