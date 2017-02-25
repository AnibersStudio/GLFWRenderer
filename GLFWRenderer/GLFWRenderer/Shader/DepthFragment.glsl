#version 430 core

in vec3 fragpos;

uniform bool islinear;
uniform vec3 lightpos;
uniform float farplane;
void main() 
{
	if (islinear)
	{
		float distance = length(fragpos - lightpos);
		distance /= farplane;
		gl_FragDepth = distance;
	}
	else
	{
		gl_FragDepth = gl_FragCoord.z;
	}
}