#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturecoord;

uniform mat4 WVP;
uniform unsigned int lightspacecount;

out vec2 texcoord;
// hasshadow - 1 is the index of this array
out vec3 lightspace[36];

void main()
{
	gl_Position = WVP * vec4(position, 1.0f);
	texcoord = texturecoord;
	for (unsigned int i = 1u; i != lightspacecount; i++)
	{
		vec4 lightspacepos = lightWVP * vec4(position);
		lightspace[i].xy = lightspacepos.xy / lightspacepos.w * 0.5 + 0.5;
		lightspace[i].z = (lightspacepos.w - plane.x) / (plane.y - plane.x);
	}
}