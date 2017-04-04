#version 430 core

uniform uint2 tilecount;

layout(std430, binding = 1) buffer DepthMinMax
{
	uint2 DepthBoundary[];
}

uint encodefloat(float i);

void main()
{
	float depthf = gl_FragCoord.z / gl_FragCoord.w;
	uint2 tilecoord = (uint2)(gl_FragCoord.xy * 0.5 + 0.5) * tilecount;
	uint index = tilecoord.x * tilecount.y + tilecoord.y;
	uint depth = encodefloat(depthf);
	atomicMin(DepthBoundary[index].x, depth);
	atomicMax(DepthBoundary[index].y, depth);
}

uint encodefloat(float i)
{
	return (uint)( i * (float)((uint)0xFFFFFFFF));
}