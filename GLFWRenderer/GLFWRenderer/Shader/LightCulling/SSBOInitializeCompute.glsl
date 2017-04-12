#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer ssbo
{
	uvec2 value[];
};

void main()
{
	uvec2 tilecoord = gl_WorkGroupID.xy;
	uint tileindex = tilecoord.x * gl_NumWorkGroups.y + tilecoord.y;
	value[tileindex].x = uint(0xFFFFFFFF);
	value[tileindex].y = 0u;
}