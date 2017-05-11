#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in float depthin[];
in flat uint lightidin[];
out float depth;
out flat uint lightid;

uniform bool ineroroutertest;

void main()
{
	vec2 tilesize = vec2(1.0f) / vec2(42.0f, 24.0f);
	vec3 base1 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w - gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
	vec3 base2 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w - gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
	float b = (tilesize.x * base1.y - tilesize.y * base1.x) / (base2.x * base1.y - base2.y * base1.x);
	float a = (tilesize.x - b * base2.x) / base1.x;
	float deltadepth = abs(a * base1.z + b * base2.z);
	//if (!ineroroutertest)
	//{
	//	deltadepth = -deltadepth;
	//}
	for (uint i = 0; i != 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		depth = depthin[i] - 0.04f;
		lightid = lightidin[i];
		EmitVertex();
	}
	EndPrimitive();
} 