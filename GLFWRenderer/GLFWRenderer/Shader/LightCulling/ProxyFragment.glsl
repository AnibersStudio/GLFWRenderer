#version 430 core

in uint lightid;

uniform uvec2 tilecount;

layout (std430, binding = 0) coherent buffer lightindexlist
{
	uvec2 lightindex[];
	//.x means start
	//.y means tail
}

layout (std430, binding = 1) buffer lightlinkedlist
{
	uvec2 lightlinked[];// lightlinked[0]/[0xFFFFFFFF] is left empty on purpose
	//.x means light id
	//.y means next node
}

layout (std430, binding = 2) atomic_counter listcounter;//Initial value: 1

void main() 
{
	uvec2 tilecoord = uvec2(gl_FragCoord.xy);
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;

	memoryBarrierBuffer();
	uint tiletail = lightindex[tileindex].y;
	uint nexttail = atomicCounterIncrement(listcounter);
	if (!tiletail)
	{
		lightindex[tileindex].x = nexttail;
	}
	lightindex[tileindex].y = nexttail;

	if (tiletail)
	{
		lightlinked[tiletail].y = nexttail;
	}
	lightlinked[nexttail].x = lightid;
}