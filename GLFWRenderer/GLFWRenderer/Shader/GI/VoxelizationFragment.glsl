#version 430 core
#extension GL_ARB_bindless_texture : enable
#define VoxelDimension 64u
#define GridCellSize 1.0f;

in vec3 fragposworld;
in vec3 fragtexcoord;
in vec3 fragnormal;
flat in uint fragmatindex;

uniform vec3 gridcenter;

struct Voxel
{
	uint colormask;
	uvec4 normalmask;
	uint occlusion;
}

struct Material
{
	vec3 ambientcolor;
	vec3 diffusecolor;
	vec3 specularcolor;
	vec3 emissivecolor;
	float shininess;
	float transparency;
};

struct ShaderTexture
{
	sampler2D handle;
	uint is;
};

struct ShaderMaterial
{
	Material material;//5 vec3s	
	ShaderTexture diffuse;
	ShaderTexture specular;
	ShaderTexture normal;
	ShaderTexture emissive;
	ShaderTexture trans;
};

layout (std430, binding = 0) buffer VoxelBuffer
{
	Voxel voxel[];
};

layout (std430, binding = 1) buffer materiallist
{
	ShaderMaterial material[];
};

static vec3 faceVectors[4] = { 
	vec3(0.0f,-0.57735026f,0.81649661f), 
	vec3(0.0f,-0.57735026f,-0.81649661f), 
	vec3(-0.81649661f,0.57735026f,0.0f), 
	vec3(0.81649661f,0.57735026f,0.0f) }
	
uint GetNormalIndex(vec3 normal, out float dotProduct)
{
	mat3x4 faceMatrix;
	faceMatrix[0] = faceVectors[0];
	faceMatrix[1] = faceVectors[1];
	faceMatrix[2] = faceVectors[2];
	faceMatrix[3] = faceVectors[3];
	vec4 dotProduct = normal * faceMatrix;
	float maximum = max(max(max(dotProduct.x, dotProduct.y)dotProduct.z), dotProduct.w);
	int index;
	if (maximum == dotProduct.x)
		index = 0;
	else if (maximum == dotProduct.y)
		index = 1;
	else if (maximum == dotProduct.z)
		index = 2;
	else
		index = 3;
	return index;
}

uint EncodeColor(vec3 color);
uint EncodeNormal(vec3 normal);

void main()
{
	vec3 base = texture().rgb;
	uint colormask = EncodeColor(base);
	float contrast = length(base.rrg - base.gbb) / (sqrt(2.0f) + base.r + base.g + base.b);
	uint ucontrast = uint(contrast * 255.0f);
	colormask |= ucontrast << 24;
	
	vec3 normal = normalize(fragnormal);
	uint normalmask = EncodeNormal(normal);
	
	float dotProduct;
	uint normalindex = GetNormalIndex(normal, dotProduct);
	uint udotProduct = uint(clamp(dotProduct, 0.0f, 1.0f) * 31.0f);
	normalmask |= udotProduct << 27;
	
	vec3 offset = (fragposworld - gridcenter) / GridCellSize;
	ivec3 voxelpos = ivec3(VoxelDimension / 2) + ivec3(offset);
	
	bvec3 isless = lessThan(voxelpos, ivec3(VoxelDimension));
	bvec3 isgreater = greaterThan(voxelpos, ivec3(-1));
	if ( isless.x && isless.y && isless.z && isgreater.x && isgreater.y && isgreater.z)
	{
		uint gridindex = voxelpos.x * VoxelDimension * VoxelDimension + voxelpos.y * VoxelDimension + voxelpos.z;
		AtomicMax(voxel[gridindex].colormask, colormask);
		AtomicMax(voxel[gridindex].normalmask[normalindex], normalmask);
		voxel[gridindex].occlusion = 1u;
	}
}


uint EncodeColor(vec3 color)
{
	return uint(color.r * 255.0f) << 16 | uint(color.g * 255.0f) << 8 | uint(color.b * 255.0f);
}

uint EncodeNormal(vec3 normal)
{
	uint res = uint(normal.x * 255.0f) << 18 | uint(normal.y * 255.0f) << 9 | uint(normal.z * 255.0f);
	if (normal.x > 0)
	{
		res |= 1u << 26;
	}
	if (normal.y > 0)
	{
		res |= 1u << 17;
	}
	if (normal.z > 0)
	{
		res |= 1u << 8;
	}
	return res;
}