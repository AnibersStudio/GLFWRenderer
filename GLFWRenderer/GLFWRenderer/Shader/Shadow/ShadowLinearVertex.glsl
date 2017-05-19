#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 WVP;
// .x near plane .y far plane
uniform vec2 plane;

out float depth;

void main()
{
	vec4 fragpos =  WVP * vec4(position, 1.0f);
	float depthf = (fragpos.w - plane.x) / (plane.y - plane.x);
	//fragpos.z = (depthf - 0.5f) * 2.0f * fragpos.w;
	depth = depthf;
	gl_Position  = fragpos;
}