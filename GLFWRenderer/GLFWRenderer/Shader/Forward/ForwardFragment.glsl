#version 430 core

layout (location = 0) out vec4 Color;

uniform sampler2D diffuse;

in vec2 texcoord;

void main() 
{
	Color = texture(diffuse, texcoord);
}