#version 430 core
layout(location = 0) in vec2 position;


out vec2 texcoord;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	texcoord = position * 0.5f + 0.5f;
}
