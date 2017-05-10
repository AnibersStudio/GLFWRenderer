#version 400

layout (location = 0) out vec4 color;

in vec3 fragcolor;

void main()
{
	color = vec4(fragcolor, 1.0f);
}