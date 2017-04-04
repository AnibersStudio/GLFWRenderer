#version 430 core

in float zinview;

void main() 
{
	gl_FragDepth = zinview;
}