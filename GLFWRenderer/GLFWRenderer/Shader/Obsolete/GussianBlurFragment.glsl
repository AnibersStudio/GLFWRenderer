#version 430 core

in vec2 texcoord;

out vec4 blurcolor;

uniform float weight[6] = {0.227967,0.193627,0.118646,0.0524476,0.0167259,0.00384807};
uniform int samplecount = 6;

uniform bool ishorizonal;

uniform sampler2D imagesampler;

void main()
{
	vec2 offset = 1.0 / textureSize(imagesampler, 0); 
    vec3 result = texture(imagesampler, texcoord).rgb * weight[0]; // current fragment's contribution
    if(ishorizonal)
    {
        for(int i = 1; i != samplecount; i++)
        {
            result += texture(imagesampler,texcoord + vec2(offset.x * i, 0.0)).rgb * weight[i];
            result += texture(imagesampler, texcoord - vec2(offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i != samplecount; ++i)
        {
			result += texture(imagesampler,texcoord + vec2(0.0, offset.y * i)).rgb * weight[i];
            result += texture(imagesampler, texcoord - vec2(0.0, offset.y * i)).rgb * weight[i];
        }
    }
    blurcolor = vec4(result, 1.0);
}
