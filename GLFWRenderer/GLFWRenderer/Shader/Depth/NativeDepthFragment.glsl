#version 430 core

void main() 
{
	gl_FragDepth = gl_FragCoord.z / gl_FragCoord.w;
}