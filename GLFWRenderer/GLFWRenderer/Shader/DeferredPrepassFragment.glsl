#version 430 core

struct Material
{
	vec3 ambientcolor;
	vec3 diffusecolor;
	vec3 specularcolor;
	vec3 emissivecolor;
	float shininess;
	float trans;
};

in vec2 fragtexcoord;
in vec3 fragnormal;
in vec3 fragtangent;
in float fragdepthinview;

layout(location = 0) out mediump3 normalstorage;
layout(location = 1) out lowp4 albedostorage;

uniform Material material
uniform bool isdiffuse;
uniform sampler2D diffusesampler;
uniform bool isspecular;
uniform sampler2D specularsampler;
uniform bool isemissive;
uniform sampler2D emissivesampler;
uniform bool isnormal;
uniform sampler2D normalsampler;
uniform bool istrans;
uniform sampler2D transsampler;

uniform float farplane;

void main()
{
	//Calculate Normal
	//Calculate Color
	//Encode Alpha
	normalstorage.xy = normal;
	normalstorage.z = shininess and alpha;
	albedostorage.xyz = albedo;
	albedostorage.w = metalness;
	
	
	
	
	gl_FragDepth = fragdepthinview / farplane;
}