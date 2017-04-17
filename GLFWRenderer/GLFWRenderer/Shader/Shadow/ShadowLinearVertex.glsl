#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 WVP;
// .x near plane .y far plane
uniform vec2 plane;

out float depth;

void main()
{
	vec4 fragdepth =  WVP * vec4(position, 1.0f);
	gl_Position  = fragdepth;
	depth = (fragdepth.w - planes.x) / (planes.y - planes.x);
}