#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 transform;

uniform mat4 WVP;
uniform uint lightoffset;
// A value < 1.0 which indicates how much tile pixel is dismatched from the divition of screen pixels. when resolution % tilesize != 0
uniform vec2 tiledismatchscale;
uniform bool ineroroutertest;
uniform uvec2 tilecount;

out flat uint lightid;
out float depth;

void main()
{
	vec4 vertexposition = WVP * transform * vec4(position, 1.0f);
	vec3 clipspace = vertexposition.xyz / vertexposition.w;
	vec2 ndc = clipspace.xy * 0.5f + 0.5f;
	ndc *= tiledismatchscale;
	clipspace.xy = (ndc - 0.5f) * 2.0f;
	gl_Position = vec4(clipspace.xy * vertexposition.w, 0.0f * vertexposition.w, vertexposition.w);
	lightid = lightoffset + gl_InstanceID;

	//translate depth
	float nativedepth = clipspace.z * 0.5f + 0.5f;
	vec4 targetposition = WVP * transform * vec4(position + normal, 1.0f);
	vec3 target = targetposition.xyz / targetposition.w;
	vec2 targetndc = target.xy * 0.5f + 0.5f;
	targetndc *= tiledismatchscale;
	target.xy = (targetndc - 0.5f) + 0.5f;
	vec3 vertnormal = target - clipspace;
	vec2 tileplanesize = vec2(2.0f) / vec2(float(tilecount.x), float(tilecount.y)) / vec2(2.0f);
	float dzdx = abs(vertnormal.x / vertnormal.z);
	float dzdy = abs(vertnormal.y / vertnormal.z);
	float depthtranslate = dzdx * tileplanesize.x + dzdy * tileplanesize.y;
	if (!ineroroutertest)
	{
		depthtranslate = - depthtranslate;
	}
	depth = nativedepth + depthtranslate / 2.0f;
}