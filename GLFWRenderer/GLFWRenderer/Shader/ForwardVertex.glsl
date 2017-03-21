#version 430 core
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texturecoord;
layout(location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out vec3 fragposition;
out vec2 texcoord;
out vec3 fragnormal;
out vec3 fragtangent;
out vec3 dlspacepos;
out vec3 slspacepos;

uniform mat4 WVP;
uniform mat4 dlLightWVP;
uniform mat4 slLightWVP;

void main()
{
	gl_Position =  WVP * vec4(position, 1.0);
	fragposition = position;
	texcoord = texturecoord;
	fragnormal = normal;
	fragtangent = tangent;
	vec4 dlposh = dlLightWVP * vec4(position, 1.0);
	dlspacepos = dlposh.xyz / dlposh.w;
	vec4 slposh = slLightWVP * vec4(position, 1.0);
	slspacepos = slposh.xyz / slposh.w;
}