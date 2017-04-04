#version 430 core

layout(location = 0) out float OMin;
layout(location = 1) out float OMax;
layout(location = 2) out float TMin;
//gl_FragDepth is TMax

uniform uint2 tilecount;

layout(std430, binding = 0) buffer DepthMinMax1
{
	uint2 DepthBoundaryO[];
}

layout(std430, binding = 1) buffer DepthMinMax2
{
	uint2 DepthBoundaryT[];
}

float decodefloat(uint i);

void main()
{
	uint2 tilecoord = (uint2)(gl_FragCoord.xy * 0.5 + 0.5) * tilecount;
	uint index = tilecoord.x * tilecount.y + tilecoord.y;
	
	OMin = decodefloat(DepthBoundaryO[index].x);
	OMax = decodefloat(DepthBoundaryO[index].y);
	TMin = decodefloat(DepthBoundaryT[index].x);
	gl_FragDepth = decodefloat(DepthBoundaryT[index].y);
}

float decodefloat(uint i)
{
	return ((float) i) / (float)((uint)(0xFFFFFFFF));
}