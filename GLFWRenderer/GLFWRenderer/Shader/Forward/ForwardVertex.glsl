#version 430 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturecoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in uint materialindex;

out vec3 fragpos;
out vec2 texcoord;
out vec3 fragnormal;
out vec3 fragtangent;
flat out uint fragmatindex;

uniform mat4 WVP;

void main()
{
	gl_Position = WVP * vec4(position, 1.0f);
	fragpos = position;
	texcoord = texturecoord;
	fragnormal = normal;
	fragtangent = tangent;
	fragmatindex = materialindex;
}