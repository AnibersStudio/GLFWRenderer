#version 430 core
#extension GL_ARB_bindless_texture : require
layout(location = 0) in vec2 position;

void main()
{
	gl_Position = vec4(position, 0.0f, 1.0f);
}