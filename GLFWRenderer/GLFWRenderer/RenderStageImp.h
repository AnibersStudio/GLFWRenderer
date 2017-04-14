#pragma once
#include "GLCommon.h"
#include "RenderVao.h"
#include "RenderFbo.h"
#include "ShaderController.h"
#include "RenderContext.h"
#include "PerFrameData.h"
#include "GLState.h"
#include "LightProxyMesh.h"
class DebugOutput
{
public:
	DebugOutput(unsigned int w, unsigned int h);
	void Draw(GLuint display);

	Vao vao;
	ShaderController displaycon;
	GLState glstate;
	unsigned int width;
	unsigned int height;
};

class PreDepthStage
{
public:
	PreDepthStage();
	void Init() {}
	void Prepare(glm::mat4 WVP);
	void Draw(GLState & oldglstate, Vao & vao, Fbo & fbo, unsigned int vertcount);
	GLState glstate;
private:
	ShaderController depthcontroller;
};

class ForwardStage
{
public:
	ForwardStage(unsigned int w, unsigned int h);

	void Init() {}
	void Prepare(PerFrameData & framedata, glm::mat4 WVP);
	void Draw(GLState & oldglstate, unsigned int vertcount);

	Vao & GetVao() { return vao; }
	Fbo & GetFbo() { return fbo; }
private:
	Vao vao;
	Fbo fbo;
	PerFrameData * data;
	ShaderController forwardcon;
	GLState glstate;
};

class LightCullingStage
{
public:
	LightCullingStage(unsigned int w, unsigned int h);
	void Init(Fbo & forawrdfbo);
	void Prepare(glm::mat4 WVP, PerFrameData framedata);
	void Draw(GLState & oldglstate, Vao & vao, unsigned int opacevertcount, unsigned int transvertcount);

	Fbo & GetMinDepth() { return mindepth; }
	Fbo & GetMaxDepth() { return maxdepth; }
	glm::uvec2 GetTileCount() { return tilecount; }
	glm::vec2 GetTileDismatchScale() { return tiledismatchscale; }
private:
	const glm::uvec2 tilesize{ 32, 32 };
	glm::uvec2 tilecount;
	glm::vec2 tiledismatchscale;

	Vao depthrangevao;
	Vao proxyvao;

	Fbo mindepth;
	Fbo maxdepth;
	Fbo * forwardfbo;

	GLState depthatomicstate;
	GLState depthrangestate;
	GLState inerproxystate;
	GLState outerproxystate;

	unsigned int pointlightcount;
	unsigned int spotlightcount;
	unsigned int inerpointlightcount;
	unsigned int inerspotlightcount;

	GLuint depthminmaxbuffer;
	GLuint pointlightindex;
	GLuint spotlightindex;
	GLuint lightlinkedlist;
	GLuint lightlinkedlistcounter;

	ShaderController depthatomicrenderer;
	ShaderController depthdownscaler;
	ShaderController depthinitializer;
	ShaderController depthrangerenderer;
	ShaderController proxyrenderer;
	ShaderController lightindexinitializer;
};