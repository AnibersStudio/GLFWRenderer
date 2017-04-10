#version 430 core

layout (location = 0) out vec4 Color;

uniform sampler2D display;
uniform vec2 winSize;

void main()
{
	Color = texture(display, gl_FragCoord.xy / winSize * 0.5 + 0.5);
}