#version 430

layout (location = 0) out vec4 color;

in float nativedepth;

void main()
{
	color = vec4(0.6f, 0.6f, 0.0f, 0.2f);
//	gl_FragDepth = clamp(nativedepth, 0.0f, 0.999999999999f);
}