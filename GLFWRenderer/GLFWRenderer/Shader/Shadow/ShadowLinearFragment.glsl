#version 430 core

in float depth;
// .x near plane .y far plane
uniform vec2 plane;
void main() 
{
	gl_FragDepth = (1 / gl_FragCoord.w - plane.x) / (plane.y - plane.x);
}