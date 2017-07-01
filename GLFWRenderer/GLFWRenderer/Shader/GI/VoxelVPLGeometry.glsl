#version 430 core

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

out uint depth;

void main()
{
	for (int i = 0; i != 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		gl_Layer = gl_InstanceID;
		depth = gl_InstanceID;
		EmitVertex();
	}
	EmitPrimitive();
}