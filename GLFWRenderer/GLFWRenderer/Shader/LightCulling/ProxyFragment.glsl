#version 430 core

in flat uint lightid;

uniform uvec2 tilecount;

layout (std430, binding = 0) coherent buffer lightindexlist
{
	uvec2 lightindex[];
	//.x means start
	//.y means tail
};

layout (std430, binding = 1) buffer lightlinkedlist
{
	uvec2 lightlinked[];// lightlinked[0]/[0xFFFFFFFF] is left empty on purpose
	//.x means light id
	//.y means next node
};

layout (binding = 2) uniform atomic_uint listcounter;//Initial value: 1
uint encodefloat(float invalue);
void main() 
{
	uvec2 tilecoord = uvec2(gl_FragCoord.xy);
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;

	lightlinked[0].x = 1u;

	memoryBarrierBuffer();
	uint tiletail = lightindex[tileindex].y;
	uint nexttail = atomicCounterIncrement(listcounter);
	if (tiletail == 0)
	{
		atomicExchange(lightindex[tileindex].x, nexttail);
	}
	atomicExchange(lightindex[tileindex].y, nexttail);

	if (tiletail > 0)
	{
		lightlinked[tiletail].y = nexttail;
	}
	lightlinked[nexttail].x = lightid;
}
uint encodefloat(float invalue)
{
	return uint(float(uint(0xFFFFFFFF)) * invalue);
}