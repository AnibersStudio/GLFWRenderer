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
	void Draw(GLuint display, GLState& oldglstate);
	void Draw(glm::mat4 WVP, std::vector<float> vertices, std::vector<glm::mat4> instances, GLState & oldglstate);
	void Draw(glm::uvec2 tilecount, GLuint lightindexbuffer, GLState & oldglstate);
	void Draw(Vao & vao, glm::mat4 WVP, unsigned int vertoffset, unsigned int vertcount, unsigned int instanceoffset, unsigned int instancecount, GLState & oldglstate, bool clear, bool face);

	Vao quadvao;
	Vao forwardvao;
	ShaderController quaddisplaycon;
	ShaderController forwarddisplaycon;
	ShaderController tiledisplaycon;
	GLState glstate;
	unsigned int width;
	unsigned int height;
};

class PreDepthStage
{
public:
	PreDepthStage(unsigned int w, unsigned int h);
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

	void Init(glm::uvec2 tilecount);
	void Prepare(PerFrameData & framedata, glm::mat4 WVP, std::vector<LightTransform> & lighttransformlist, std::tuple<unsigned int, unsigned int, unsigned int> shadowcount, vec3 eye);
	void Draw(GLState & oldglstate, unsigned int vertcount, std::tuple<GLuint, GLuint, GLuint> lightlinked);

	Vao & GetVao() { return vao; }
	Fbo & GetFbo() { return fbo; }
private:
	Vao vao;
	Fbo fbo;
	PerFrameData * data;
	ShaderController forwardcon;
	GLState glstate;

	GLuint lighttransformlistbuffer;
	GLuint directionallightbuffer;
	GLuint pointlightbuffer;
	GLuint spotlightbuffer;
};

class LightCullingStage
{
public:
	LightCullingStage(unsigned int w, unsigned int h);
	void Init() {}
	void Prepare(glm::mat4 WVP, PerFrameData framedata);
	void Draw(GLState & oldglstate, Vao & vao, Fbo & fbo, unsigned int opacevertcount, unsigned int transvertcount);

	glm::uvec2 GetTileCount() { return tilecount; }
	glm::vec2 GetTileDismatchScale() { return tiledismatchscale; }
	std::tuple<GLuint, GLuint, GLuint> GetLightIndexAndLinked() { return {pointlightindex, spotlightindex, lightlinkedlist}; }
private:
	unsigned int width, height;
	const glm::uvec2 tilesize{ 32, 32 };
	glm::uvec2 tilecount;
	glm::vec2 tiledismatchscale;

	Vao depthrangevao;
	Vao proxyvao;

	Fbo proxydepth;

	GLState depthatomicstate;
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
	ShaderController proxyrenderer;
	ShaderController lightindexinitializer;
};

class ShadowStage
{
public:
	ShadowStage();

	void Init();
	void Prepare(unsigned int pointshadow, unsigned int spotshadow, PerFrameData & framedata, glm::vec3 eye);
	void Draw(GLState & oldglstate, Vao & vao, unsigned int opacevertcount);

	std::tuple<unsigned int, unsigned int, unsigned int> GetShadowCount() { return shadowcount; };
	std::vector<LightTransform> & GetTransfromList() { return transformlist; };

	std::vector<Fbo> & GetFboDirectional() { return directionalfbo; }
	std::vector<std::vector<Fbo>> & GetFboPoint() { return pointfbo; }
	std::vector<Fbo> & GetFboSpot() { return spotfbo; }
	std::vector<LightTransform> & GetLightTransformList() { return transformlist; }

//private:
	/// <summary> Option = 0: middlep 1: highp 2: lowp</summary>
	std::pair<Fbo&, Fbo&> GetMiddleFbo(int option);
	void InitMiddleFbo();
	void SetShadowedLight(PerFrameData & framedata);
	void CalculateVP(PerFrameData & framedata, glm::vec3 eye);
	unsigned int dmaxshadow;
	unsigned int pmaxshadow;
	unsigned int smaxshadow;
	
	std::vector<LightTransform> pointvplist;
	std::vector<LightTransform> transformlist;
	std::vector<glm::vec3> pointposlist;

	std::vector<Fbo> directionalfbo;
	std::vector<std::vector<Fbo>> pointfbo;
	std::vector<Fbo> spotfbo;
	std::tuple<unsigned int, unsigned int, unsigned int> shadowcount;
	const unsigned int batchcount = 6;

	std::vector<Fbo> highpmiddlefbo;
	std::vector<Fbo> highpsinglebluredfbo;
	std::vector<Fbo> middlefbo;
	std::vector<Fbo> singlebluredfbo;
	std::vector<Fbo> lowpmiddlefbo;
	std::vector<Fbo> lowpsinglebluredfbo;

	GLState linearstate;
	GLState linearhighpstate;
	GLState linearlowpstate;
	GLState blurstate;
	GLState blurhighpstate;
	GLState blurlowpstate;
	ShaderController lineardepthcon;
	ShaderController omnidirectionalcon;
	ShaderController logspaceblurcon;

	Vao quadvao;

	unsigned int highindex = 0;
	unsigned int index = 0;
	unsigned int lowindex = 0;

	const float maxdrange;
	const float maxprange;
	const float maxsrange;
};