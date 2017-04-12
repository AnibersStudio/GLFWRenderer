#version 430 core

in vec2 texcoord;

uniform sampler2D screenimagesampler;
uniform bool isHDR;
uniform float exposure;
uniform float gamma;

out vec4 pixelcolor;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
	vec3 hdrcolor = texture(screenimagesampler, texcoord).rgb;
	hdrcolor = vec3(1.0) - exp(-hdrcolor * exposure);

	vec3 mapped = hdrcolor;
	if (isHDR)
	{
		float ExposureBias = 1.0f;
		vec3 curr = Uncharted2Tonemap(ExposureBias*hdrcolor);

		vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));
		mapped = curr*whiteScale;
	}
	mapped = pow(mapped, vec3(1/gamma));

	pixelcolor = vec4(mapped, 1.0);
}
