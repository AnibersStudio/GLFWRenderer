#version 430 core

uniform sampler2D depthsampler;
uniform uint2 tilecount;

layout(std430, binding = 0) buffer DepthMinMax
{
	uint2 DepthBoundary[];
}

uint encodefloat(float i);

void main()
{
	float depthf = texture(depthsampler, gl_FragCoord.xy * 0.5 + 0.5).x;
	uint2 tilecoord = uint2((uint)(gl_FragCoord.x * tilecount.x), (uint)(gl_FragCoord.y * tilecount.y));
	uint index = tilecoord.x * tilecount.y + tilecoord.y;
	uint depth = encodefloat(depthf);
	atomicMin(DepthBoundary[index].x, depth);
	atomicMax(DepthBoundary[index].y, depth);
}

uint encodefloat(float i)
{
	return (uint)( i * (float)((uint)0xFFFFFFFF));
}