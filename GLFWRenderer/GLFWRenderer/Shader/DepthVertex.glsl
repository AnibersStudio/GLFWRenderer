#version 430 core
layout (location = 0) in vec3 position;

out vec3 fragpos;

uniform mat4 LightWVP;

void main()
{
	vec4 position = LightWVP * vec4(position, 1.0f);
	fragpos = position.xyz / position.w;
	gl_Position = position;
}