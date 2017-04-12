#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer ssbo1
{
	uvec2 value1[];
};

layout (std430, binding = 1) buffer ssbo2
{
	uvec2 value2[];
};

void main()
{
	uvec2 tilecoord = gl_WorkGroupID.xy;
	uint tileindex = tilecoord.x * gl_NumWorkGroups.y + tilecoord.y;
	value1[tileindex].x = uint(0xFFFFFFFF);
	value1[tileindex].y = 0u;
	value2[tileindex].x = uint(0xFFFFFFFF);
	value2[tileindex].y = 0u;
}