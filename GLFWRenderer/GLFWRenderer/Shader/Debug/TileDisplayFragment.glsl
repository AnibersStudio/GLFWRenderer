#version 430 core

layout (location = 0) out vec4 Color;

uniform uvec2 tilecount;
uniform uvec2 tilesize = uvec2(32u, 32u);

layout (std430, binding = 0) buffer lightindexlist
{
	uvec2 lightindex[];
	//.x means start
	//.y means tail
};

void main()
{
	uvec2 tilecoord = uvec2(gl_FragCoord.xy) / tilesize;
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;

	if (lightindex[tileindex].x != 0xFFFFFFFF)
	{
		Color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		Color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}