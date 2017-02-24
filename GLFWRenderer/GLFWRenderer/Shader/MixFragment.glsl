#version 430 core

in vec2 texcoord;

out vec4 mixcolor;

uniform sampler2D colorsampler;
uniform sampler2D brightsampler;

void main()
{
	mixcolor = vec4(texture(colorsampler, texcoord).rgb + texture(brightsampler, texcoord).rgb, 1.0);
}
