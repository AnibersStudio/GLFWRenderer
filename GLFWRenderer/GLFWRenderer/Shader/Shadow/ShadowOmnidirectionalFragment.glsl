#version 430 core

in vec3 fragpos;

// .x near plane .y far plane
uniform vec2 plane;
uniform vec3 center;

void main()
{
	gl_FragDepth = (length(center - fragpos) - plane.x) / (plane.y - plane.x);
}