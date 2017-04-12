#version 430 core
#extension GL_ARB_shader_draw_parameters : require
#extension GL_ARB_bindless_texture : require
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out vec2 fragtexcoord;
out vec3 fragnormal;
out vec3 fragtangent;
out float fragdepthinview;
out uint materialindex;

uniform mat4 WVP;

void main()
{
	gl_Position =  WVP * vec4(position, 1.0);
	fragtexcoord = texcoord;
	fragnormal = normal;
	fragtangent = tangent;
	fragdepthinview = vec4(WVP * vec4(position, 1.0)).w;

	materialindex = gl_DrawIDARB;
}