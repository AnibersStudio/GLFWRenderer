#version 430 core
#define VoxelDimension 64u

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

//in vec2 texcoord[3];
//in vec3 vertexnormal[3];
//flat in uint fragmatindex[3];

out vec3 fragposworld;
//out vec3 fragtexcoord;
//out vec3 fragnormal;
//flat out uint fragmaterialindex;

uniform mat4 ViewProjMatrix[3];

static vec3 viewDirection[3] = { 
	vec3(0.0f, 0.0f, -1.0f), 
	vec3(-1.0f, 0.0f, 0.0f), 
	vec3(0.0f, -1.0f, 0.0f)};
int GetViewIndex(vec3 normal)
{
	mat3 directionMatrix;
	directionMatrix[0] = -viewDirection[0];
	directionMatrix[1] = -viewDirection[1];
	directionMatrix[2] = -viewDirection[2];
	vec3 dotproduct = abs(normal * directionMatrix);
	float maximum = max(max(dotproduct.x, dotproduct.y)dotproduct.z);
	int index;
	if (maximum == dotproduct.x)
		index = 0;
	else if (maximum == dotproduct.y)
		index = 1;
	else
		index = 2;
	return index;
}
void main()
{
	vec3 normal = normalize(fragnormal[0] + fragnormal[1] + fragnormal[2]);
	int viewindex = GetViewIndex(normal);
	
	vec4 outputpos[3];
	for (int i = 0; i != 3; i++)
	{
		outputpos[i] = ViewProjMatrix[viewindex] * gl_in[i].gl_Position;
	}
	vec2 side0N = normalize(outputpos[1].xy - outputpos[0].xy);
	vec2 side1N = normalize(outputpos[2].xy - outputpos[1].xy);
	vec2 side2N = normalize(outputpos[0].xy - outputpos[2].xy);
	float texelsize = 1.0f / float(2u * VoxelDimension);
	outputpos[0].xy += normalize(side2N - side0N) * texelsize;
	outputpos[1].xy += normalize(side0N - side1N) * texelsize;
	outputpos[2].xy += normalize(side1N - side2N) * texelsize;
	
	for (int i = 0; i != 3; i++)
	{
		gl_Position = outputpos[i];
		fragposworld = gl_in[i].gl_Position.xyz;
		//fragnormal = vertexnormal[i];
		//fragtexcoord = texcoord[i];
		//fragmaterialindex = fragmatindex[i];
		EmitVertex();
	}
	EmitPrimitive();
}
