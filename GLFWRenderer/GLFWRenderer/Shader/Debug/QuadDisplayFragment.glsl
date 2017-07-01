#version 430 core

layout (location = 0) out vec4 Color;

uniform sampler2D display;

in vec2 texcoord;

void main()
{
	vec4 color = texture(display, texcoord);
	Color = color;
}