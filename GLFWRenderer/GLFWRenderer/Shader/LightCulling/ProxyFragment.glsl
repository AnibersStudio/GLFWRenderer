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

	memoryBarrierBuffer();
	uint nexttail = atomicCounterIncrement(listcounter);
	uint tiletail = atomicExchange(lightindex[tileindex].y, nexttail);
	if (tiletail == 0u)
	{
		// No need for atomic
		//atomicExchange(lightindex[tileindex].x, nexttail);
		lightindex[tileindex].x = nexttail;
	}

	lightlinked[nexttail].x = lightid;
	if (tiletail != 0u)
	{
		lightlinked[tiletail].y = nexttail;
	}
}
uint encodefloat(float invalue)
{
	return uint(float(uint(0xFFFFFFFF)) * invalue);
}