#version 430 core
layout (location = 0) in vec3 position;
layout (location = 2) in mat4 transform;

uniform mat4 WVP;

out float nativedepth;

void main()
{
	vec4 worldposition = transform * vec4(position, 1.0f);
	worldposition /= worldposition.w;
	vec4 vertexposition = WVP * worldposition;
	vec3 clipspace = vertexposition.xyz / vertexposition.w;
	gl_Position = vec4(clipspace.xyz * vertexposition.w, vertexposition.w);

//	nativedepth = clipspace.z * 0.5f + 0.5f;
}