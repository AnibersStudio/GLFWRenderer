#version 430 core

uniform sampler2D depthsampler;
uniform uvec2 tilecount;
uniform uvec2 winSize;

uniform image2D depthimage;

void main()
{
	uvec2 tilecoord = uint2((uint)(gl_FragCoord.x / winSize.x * tilecount.x), (uint)(gl_FragCoord.y /winSize.y * tilecount.y));
	uint depth = imageLoad();
	atomicMin(DepthBoundary[index].x, depth);
	atomicMax(DepthBoundary[index].y, depth);
}