#version 430 core
#define SAMPLE_COUNT 3
#define C_VALUE 400.0f
#define E_VALUE 2.71828f
uniform float blurkernel[SAMPLE_COUNT] = {0.4219f, 0.3584f, 0.2195f};
uniform vec2 axispara[2] = {{1.0f, 0.0f}, {0.0f, 1.0f}};

uniform sampler2D shadowsampler;
uniform unsigned int bluraxis;

void main()
{
	vec2 texelsize = 1.0 / vec2(textureSize(shadowsampler, 0));
    vec2 texcoord = gl_FragCoord.xy * texelsize;
	
	float d = texture(shadowsampler, texcoord).x;
	float sigma = blurkernel[0];
    for(int i = 1; i != SAMPLE_COUNT; ++i)
    {
		float w = blurkernel[i];
		float dpositive = texture(shadowsampler, texcoord + i * texelsize * axispara[bluraxis]).x;
		float dnegative = texture(shadowsampler, texcoord - i * texelsize * axispara[bluraxis]).x;
		dpositive -= d;
		dnegative -= d;
		sigma += blurkernel[i] * (exp(C_VALUE * dpositive) + exp(C_VALUE * dnegative));
    }
	sigma = log(sigma) / log(E_VALUE);
	sigma /= C_VALUE;
	
	gl_FragDepth = d + sigma;
}
