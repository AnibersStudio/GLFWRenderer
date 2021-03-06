#version 430 core

uniform uvec2 tilesize;
uniform uvec2 tilecount;

layout (std430, binding = 0) coherent buffer depthrange
{
	uvec2 value[];
};

uint encodefloat(float invalue);
float decodeuint(uint invalue);

void main()
{
	uvec2 tilecoord = uvec2(gl_FragCoord.xy) / tilesize;
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	float depthf = gl_FragCoord.z;
	uint depth = encodefloat(depthf);
	memoryBarrierBuffer();
	atomicMin(value[tileindex].x, depth);
	atomicMax(value[tileindex].y, depth);
}

uint encodefloat(float invalue)
{
	return uint(float(uint(0xFFFFFFFF)) * invalue);
}
float decodeuint(uint invalue)
{
	return float(invalue) / float(uint(0xFFFFFFFF));
}