#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturecoord;

uniform mat4 WVP;

out vec2 texcoord;

void main()
{
	gl_Position = WVP * vec4(position, 1.0f);
	texcoord = texturecoord;
}