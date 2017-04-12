#version 430 core

uniform uvec2 tilecount;
uniform uint minormax;

layout (std430, binding = 0) buffer depthrange
{
	uvec2 value[];
};

uint encodefloat(float invalue);
float decodeuint(uint invalue);

void main()
{
	uvec2 tilecoord = uvec2(gl_FragCoord.xy);
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	uint depthu = minormax * value[tileindex].x + (1 - minormax) * value[tileindex].y;
	float depth = decodeuint(depthu);
	
	gl_FragDepth = depth;
}

uint encodefloat(float invalue)
{
	return uint(float(uint(0xFFFFFFFF)) * invalue);
}
float decodeuint(uint invalue)
{
	return float(invalue) / float(uint(0xFFFFFFFF));
}