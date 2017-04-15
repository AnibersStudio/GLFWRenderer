#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in mat4 transform;

uniform mat4 WVP;
uniform uint instanceoffset;
// A value < 1.0 which indicates how much tile pixel is dismatched from the divition of screen pixels. when resolution % tilesize != 0
uniform vec2 tiledismatchscale;
uniform bool inerclamp;

out flat uint lightid;
out float depth;

void main()
{
	vec4 vertexposition = WVP * transform * vec4(position, 1.0f);
	vec3 clipspace = vertexposition.xyz / vertexposition.w;
	vec2 ndc = clipspace.xy * 0.5f + 0.5f;
	ndc *= tiledismatchscale;
	clipspace.xy = (ndc - 0.5f) * 2.0f;
	if (inerclamp)
	{
		clipspace.z = min(clipspace.z, 1.0f);
	}
	else
	{
		clipspace.z = max(clipspace.z, -1.0f);
	}
	gl_Position = vec4(clipspace, 1.0f);
	lightid = instanceoffset + gl_InstanceID;
}