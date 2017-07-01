#version 430 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturecoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in uint materialindex;

out vec2 texcoord;
out vec3 vertexnormal;
flat out uint fragmatindex;

void main()
{
	gl_Position = vec4(position, 1.0f);
	texcoord = texturecoord;
	vertexnormal = normal;
}