#pragma once
#include "GLCommon.h"
#include "RenderFbo.h"
#include "RenderVao.h"
#include "GLState.h"
#include "ShaderController.h"
#include "BufferObjectSubmiter.h"
class Bloomer
{
public:
	Bloomer(unsigned int w, unsigned int h);
	void Init(Fbo& source);
	void Prepare();
	void Draw(GLState & oldstate);
	Fbo& Result() { return result; }
private:
	GLState state;
	Fbo middle;
	Fbo result;
	Vao quadvao;
	ShaderController bloomcon;
	GLuint64 sourcehandle;
};

class EyeAdapter
{
public:
	EyeAdapter(unsigned int w, unsigned int h);
	void Init(Fbo& source);
	void Prepare();
	void Draw(GLState & oldstate);
	Fbo& Result() { return middlelist[middlelist.size() - 1]; }
private:
	Vao quadvao;
	std::vector<Fbo> middlelist;
	std::vector<GLState> statelist;
	ShaderController eyeadaptcon;
	GLuint64 sourcehandle;
};

class ToneMapper
{
public:
	ToneMapper(unsigned int w, unsigned int h);
	void Init(Fbo& source, Fbo& bloom, Fbo& exposure);
	void Prepare(bool istonemap, bool isbloom, bool iseyeadapt, float bloomratio = 0.0f, float bloomthreshold = 0.0f, float eyeadaptfreshratio = 0.0f, float gamma = 2.2f);
	void Draw(GLState & oldstate);
private:
	Vao quadvao;
	GLState state;
	ShaderController tonemapcon;
	GLuint prevcolorbuffer;
};