#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 LightWVP;

void main()
{
	gl_Position = LightWVP * vec4(position, 1.0f);
}