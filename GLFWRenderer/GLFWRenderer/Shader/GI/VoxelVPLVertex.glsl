#version 430 core

layout(location = 0) in vec2 position;

out uint instanceid;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	instanceid = gl_InstanceID;
}