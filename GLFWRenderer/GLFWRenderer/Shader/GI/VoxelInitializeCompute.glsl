#version 430 core
#define VoxelDimension 64u
layout (local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

struct Voxel
{
	uint colormask;
	uvec4 normalmask;
	uint occlusion;
}

layout (std430, binding = 0) buffer VoxelBuffer
{
	Voxel voxel[];
};

void main()
{
	uvec3 voxeloffset = gl_GlobalInvocationID;
	uint voxelindex = voxeloffset.x * VoxelDimension * VoxelDimension + voxeloffset.y * VoxelDimension + voxeloffset.z;
	voxel[voxelindex].colormask = 0u;
	voxel[voxelindex].normalmask = uvec4(0u);
	voxel[voxelindex].occlusion = 0u;
}