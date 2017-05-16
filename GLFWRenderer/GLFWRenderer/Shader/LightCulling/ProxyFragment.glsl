#version 430 core

in flat uint lightid;

uniform uvec2 tilecount;
uniform bool ineroroutertest;

layout (location = 0) out vec4 color;

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

layout (std430, binding = 2) buffer depthrangebuffer
{
	uvec2 depthrange[];
};

layout (binding = 2) uniform atomic_uint listcounter;//Initial value: 1

uint encodefloat(float invalue);
float decodeuint(uint invalue);

void main() 
{
	uvec2 tilecoord = uvec2(uint(gl_FragCoord.x), uint(gl_FragCoord.y));
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	if (ineroroutertest)
	{
		if (gl_FragCoord.z >= decodeuint(depthrange[tileindex].x))
		{
			uint nexttail = atomicCounterIncrement(listcounter);
			memoryBarrierBuffer();
			uint tiletail = atomicExchange(lightindex[tileindex].y, nexttail);
			if (tiletail == 0u)
			{
				lightindex[tileindex].x = nexttail;
			}
			lightlinked[nexttail].x = lightid;
			if (tiletail != 0u)
			{
				lightlinked[tiletail].y = nexttail;
			}
		}
	}
	else
	{
		if (gl_FragCoord.z <= decodeuint(depthrange[tileindex].y))
		{
			uint nexttail = atomicCounterIncrement(listcounter);
			memoryBarrierBuffer();
			uint tiletail = atomicExchange(lightindex[tileindex].y, nexttail);
			if (tiletail == 0u)
			{
				lightindex[tileindex].x = nexttail;
			}
			lightlinked[nexttail].x = lightid;
			if (tiletail != 0u)
			{
				lightlinked[tiletail].y = nexttail;
			}
		}
	}
}

uint encodefloat(float invalue)
{
	return uint(float(uint(0xFFFFFFFF)) * invalue);
}
float decodeuint(uint invalue)
{
	return float(invalue) / float(uint(0xFFFFFFFF));
}