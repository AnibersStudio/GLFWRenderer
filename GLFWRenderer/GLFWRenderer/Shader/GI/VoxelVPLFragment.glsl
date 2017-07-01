#version 430 core
#define PI 3.14159f
#define VoxelDimension 64u
#define GridCellSize 1.0f;

layout (location = 0) out vec4 redcoe;
layout (location = 1) out vec4 greencoe;
layout (location = 2) out vec4 bluecoe;

in uint depth;

uniform vec3 gridcenter;

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

vec4 ClampedCosineSHCoeffs(vec3 dir) 
{ 
	vec4 coeffs; 
	coeffs.x = PI/(2.0f*sqrt(PI)); 
	coeffs.y = -((2.0f*PI)/3.0f)*sqrt(3.0f/(4.0f*PI)); 
	coeffs.z = ((2.0f*PI)/3.0f)*sqrt(3.0f/(4.0f*PI)); 
	coeffs.w = -((2.0f*PI)/3.0f)*sqrt(3.0f/(4.0f*PI)); 
	coeffs.wyz *= dir; 
	return coeffs; 
}

vec3 GetClosestNormal(uvec4 normalMasks, vec3 direction, out float dotProduct) 
{
	mat4x3 normalMatrix; 
	normalMatrix[0] = DecodeNormal(normalMasks.x); //DECODE NORMAL
	normalMatrix[1] = DecodeNormal(normalMasks.y); 
	normalMatrix[2] = DecodeNormal(normalMasks.z);
	normalMatrix[3] = DecodeNormal(normalMasks.w); 
	vec4 dotProducts = normalMatrix * direction; 
	float maximum = max (max(dotProducts.x,dotProducts.y), max(dotProducts.z,dotProducts.w));
	int index; 
	if(maximum==dotProducts.x) 
		index = 0; 
	else if(maximum==dotProducts.y) 
		index = 1; 
	else if(maximum==dotProducts.z) 
		index = 2; 
	else 
		index = 3;
	dotProduct = dotProducts[index]; 
	return normalMatrix[index];
}

void main()
{
	uvec3 voxelpos = uvec3(gl_FragCoord.xy, depth);
	uint gridindex = voxelpos.x * VoxelDimension * VoxelDimension + voxelpos.y * VoxelDimension + voxelpos.z;
	
	Voxel currentvoxel = voxel[gridindex];
	if (currentvoxel.occlusion)
		discard;
	
	ivec3 offset = voxelpos - ivec3(VoxelDimension / 2u);
	vec3 position = vec3(offset) * GridCellSize + gridcenter;
	
	vec3 albedo = DecodeColor(voxel.colormask);//DECODE COLOR
	
	float ndotl;
	vec3 normal = GetClosestNormal(voxel.normalmask, lightdirection, ndotl);//LIGHT DIRECTION
	
	vec3 diffuse = CalDiffuse()//To DO 
	float shadowfactor = ESM() //TO DO
	
	vec4 coeffs = ClampedCosineSHCoeffs(normal);
	redcoe = coeffs * diffuse.r;
	greencoe = coeffs * diffuse.g;
	bluecoe = coeffs * diffuse.b;
}