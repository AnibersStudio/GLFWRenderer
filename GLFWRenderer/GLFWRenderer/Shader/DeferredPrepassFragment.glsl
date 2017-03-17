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

layout(location = 0) out mediump vec3 normalstorage;
layout(location = 1) out lowp vec4 albedostorage;
layout(location = 2) out lowp vec4 emmisivestorage;

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
	normalstorage.z = shininess;
	albedostorage.xyz = albedo;
	albedostorage.w = metalness;
	
	
	
	
	gl_FragDepth = fragdepthinview / farplane;
}