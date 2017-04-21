#version 430 core

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform uvec2 resolution;
uniform sampler2D depthsampler;

layout (std430, binding = 0) coherent buffer depthrange
{
	uvec2 value[];
};

layout (std430, binding = 1) buffer testbuffer
{
	float test[];
};

uint encodefloat(float invalue);
float decodeuint(uint invalue);

void main()
{
	uvec2 tilecoord = gl_WorkGroupID.xy;
	uvec2 imagecoord = gl_GlobalInvocationID.xy;
	uint tileindex = tilecoord.x * gl_NumWorkGroups.y + tilecoord.y;
	if (imagecoord.x < resolution.x && imagecoord.y < resolution.y)
	{
		float depthf = texture(depthsampler, vec2(imagecoord) / vec2(resolution)).r;
		if (depthf < 1.0f)
		{
			uint depth = encodefloat(depthf);
			groupMemoryBarrier();
			atomicMin(value[tileindex].x, depth);
			atomicMax(value[tileindex].y, depth);

			//if (value[tileindex].y == depth)
			//{
			//	test[tileindex]
			//}
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