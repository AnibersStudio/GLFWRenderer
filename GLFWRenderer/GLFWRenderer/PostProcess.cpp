#include "PostProcess.h"

Bloomer::Bloomer(unsigned int w, unsigned int h):
	quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
	middle(Fbo{ {w, h},{{ GL_COLOR_ATTACHMENT0_EXT, GL_RGB, GL_RGB16F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE }} } } }),
	result(Fbo{ {w, h},{{ GL_COLOR_ATTACHMENT0_EXT, GL_RGB, GL_RGB16F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE }} } } }),
	bloomcon(ShaderController{ {{ "Shader/PostProcess/GussianBlurVertex.glsl", GL_VERTEX_SHADER }, {"Shader/PostProcess/GussianBlurFragment.glsl",GL_FRAGMENT_SHADER}}, {{"image", GL_UNSIGNED_INT64_ARB}, {"ishorizonal", GL_UNSIGNED_INT}} })
{
	state.w = w, state.h = h;
	state.depthtest = false;
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad), sizeof(quad));
}

void Bloomer::Init(Fbo & source)
{
	sourcehandle = source.GetColorHandle(0);
}

void Bloomer::Prepare()
{
	bloomcon.Clear();
}

void Bloomer::Draw(GLState & oldstate)
{
	state.HotSet(oldstate);
	quadvao.Bind();

	bloomcon.Draw();
	bloomcon.Set("ishorizonal", boost::any(1u));
	bloomcon.Set("image", boost::any(sourcehandle));
	middle.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	bloomcon.Set("ishorizonal", boost::any(0u));
	bloomcon.Set("image", boost::any(middle.GetColorHandle(0)));
	result.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

EyeAdapter::EyeAdapter(unsigned int w, unsigned int h):
quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
eyeadaptcon(ShaderController{ { { "Shader/PostProcess/ExposureDownscaleVertex.glsl", GL_VERTEX_SHADER },{ "Shader/PostProcess/ExposureDownscaleFragment.glsl",GL_FRAGMENT_SHADER } },{ { "image", GL_UNSIGNED_INT64_ARB }} })
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad), sizeof(quad));

	unsigned int fbow = w, fboh = h;
	for (fbow = static_cast<unsigned int>(glm::ceil(fbow / 2.0f)), fboh = static_cast<unsigned int>(glm::ceil(fboh / 2.0f)); fbow > 1 || fboh > 1; fbow = static_cast<unsigned int>(glm::ceil(fbow / 2.0f)), fboh = static_cast<unsigned int>(glm::ceil(fboh / 2.0f)))
	{
		GLState state;
		state.w = fbow, state.h = fboh; state.depthtest = false;
		statelist.push_back(state);

		Fbo fbo({fbow, fboh}, { { GL_COLOR_ATTACHMENT0_EXT, GL_RGB, GL_RGB16F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(0.0f) } });
		middlelist.push_back(std::move(fbo));
	}
	GLState state;
	state.w = fbow, state.h = fboh; state.depthtest = false;
	statelist.push_back(state);

	Fbo fbo({ fbow, fboh }, { { GL_COLOR_ATTACHMENT0_EXT, GL_RGB, GL_RGB16F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(0.0f) } });
	middlelist.push_back(std::move(fbo));
}

void EyeAdapter::Init(Fbo & source)
{
	sourcehandle = source.GetColorHandle(0);
}

void EyeAdapter::Prepare()
{
	eyeadaptcon.Clear();
}

void EyeAdapter::Draw(GLState & oldstate)
{
	quadvao.Bind();
	eyeadaptcon.Draw();

	GLuint64 lasthandle = sourcehandle;
	for (int i = 0; i != middlelist.size(); i++)
	{
		statelist[i].HotSet(oldstate);
		eyeadaptcon.Set("image", boost::any(lasthandle));
		middlelist[i].Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		lasthandle = middlelist[i].GetColorHandle(0);
	}
}

ToneMapper::ToneMapper(unsigned int w, unsigned int h):
	quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
	tonemapcon(ShaderController{ { { "Shader/PostProcess/ToneMappingVertex.glsl", GL_VERTEX_SHADER },{ "Shader/PostProcess/ToneMappingFragment.glsl",GL_FRAGMENT_SHADER } },{ { "image", GL_UNSIGNED_INT64_ARB },{ "bloom", GL_UNSIGNED_INT64_ARB },{ "exposure", GL_UNSIGNED_INT64_ARB }, 
		{"istonemap", GL_UNSIGNED_INT}, {"isbloom", GL_UNSIGNED_INT}, {"iseyeadapt", GL_UNSIGNED_INT}, {"bloomratio", GL_FLOAT},{ "bloomthreshold", GL_FLOAT },{"exposurefreshrate", GL_FLOAT}, {"gamma", GL_FLOAT}, {"prevcolorbuffer", GL_SHADER_STORAGE_BUFFER} } })
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad), sizeof(quad));

	state.w = w; state.h = h;
	state.depthtest = false;

	prevcolorbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::vec3));
	glm::vec3 avgcolor(0.5f);
	BufferObjectSubmiter::GetInstance().SetData(prevcolorbuffer, &avgcolor[0], sizeof(glm::vec3), GL_STATIC_DRAW);
}

void ToneMapper::Init(Fbo & source, Fbo & bloom, Fbo & exposure)
{
	tonemapcon.Set("image", boost::any(source.GetColorHandle(0)));
	tonemapcon.Set("bloom", boost::any(bloom.GetColorHandle(0)));
	tonemapcon.Set("exposure", boost::any(exposure.GetColorHandle(0)));
}

void ToneMapper::Prepare(bool istonemap, bool isbloom, bool iseyeadapt, float bloomratio, float bloomthreshold, float eyeadaptfreshrate, float gamma)
{
	tonemapcon.Clear();
	tonemapcon.Set("istonemap", boost::any(static_cast<GLuint> (istonemap)));
	tonemapcon.Set("isbloom", boost::any(static_cast<GLuint> (isbloom)));
	tonemapcon.Set("iseyeadapt", boost::any(static_cast<GLuint> (iseyeadapt)));
	tonemapcon.Set("bloomratio", boost::any(bloomratio));
	tonemapcon.Set("bloomthreshold", boost::any(bloomthreshold));
	tonemapcon.Set("exposurefreshrate", boost::any(eyeadaptfreshrate));
	tonemapcon.Set("gamma", boost::any(gamma));
}

void ToneMapper::Draw(GLState & oldstate)
{
	state.HotSet(oldstate);
	quadvao.Bind();
	tonemapcon.Set("prevcolorbuffer", boost::any(prevcolorbuffer));
	tonemapcon.Draw();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
