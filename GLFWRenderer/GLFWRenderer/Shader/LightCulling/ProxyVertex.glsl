#version 430 core
#extension GL_ARB_shader_draw_parameters : require
layout (location = 0) in vec3 position;
layout (location = 1) in mat4 transform;

uniform mat4 WVP;
uniform uint instanceoffset[4];

out uint lightid;

void main()
{
	gl_Position = WVP * transform * vec4(position, 1.0f);
	lightid = instanceoffset[drawid] + gl_InstanceID;
}