#version 430 core

layout (location = 0) out vec4 Color;

uniform sampler2D display;
uniform uvec2 winSize;

void main()
{
	Color = texture(display, gl_FragCoord.xy / vec2(winSize.x, winSize.y) * 0.5 + 0.5);
}