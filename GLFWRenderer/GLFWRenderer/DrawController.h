#pragma once
#include "Model.h"
#include "LightShaderController.h"
#include "Light.h"
#include <random>

#define SIZE30M (30 * 1024 * 1024 / sizeof(Vertex) / 3 * 3 )

enum EyeAdaptTech {EyeAdaptOff, EyeAdaptReadback, EyeAdaptRenderToTex};

struct DrawContext
{
	glm::mat4 W;
	glm::mat4 V;
	glm::mat4 P;
	glm::vec3 Eye;
	float gamma = 2.2f;
	bool isHDR = false;
	EyeAdaptTech isEyeAdapt = EyeAdaptOff;
	unsigned int Bloom = 0;
	bool isShadow = false;
};

enum DepthBufferType
{
	DepthBufferT, DepthTextureT, DepthCubeMapT, None
};

struct FboSetting
{
	FboSetting(unsigned int w, unsigned int h, unsigned int colorbuffernum = 1U, DepthBufferType depthtype = DepthBufferT, GLenum depthformat = GL_DEPTH_COMPONENT,
		GLenum colorformat = GL_RGBA, GLenum colortexfilter = GL_NEAREST, GLenum colorclamp = GL_CLAMP_TO_EDGE, vec4 bordercolor = vec4(1.0))
		: ColorBufferCount(colorbuffernum), DepthType(depthtype), DepthFormat(depthformat), ColorFormat(colorformat), ColorTexFilter(colortexfilter), ColorClamp(colorclamp), BorderColor(bordercolor), Width(w), Height(h) {}
	unsigned int ColorBufferCount;
	DepthBufferType DepthType;
	GLenum DepthFormat;
	GLenum ColorFormat;
	GLenum ColorTexFilter;
	GLenum ColorClamp;
	vec4 BorderColor;
	unsigned int Width;
	unsigned int Height;
};

struct FboStruct
{
	FboStruct() : FrameBufferObject(0), Setting(0, 0, 1, DepthBufferT), DepthComponent(0){}
	FboStruct(FboSetting setting);
	void BindFrameBuffer() const;
	void BindFrameBufferForDepth() const;
	void BindFrameBufferForDepth(int i) const;
	GLuint FrameBufferObject;
	std::vector<GLuint> ColorBuffer;
	GLuint DepthComponent;
	FboSetting Setting;
};

struct VaoSetting
{
	VaoSetting(unsigned int numofvalues, GLenum valuetype, size_t valuesize) : NumofValues(numofvalues), ValueType(valuetype), ValueSize(valuesize) {}
	unsigned int NumofValues;
	GLenum ValueType;
	size_t ValueSize;
};

struct VaoStruct
{
	VaoStruct(const std::vector<VaoSetting> & setting, GLenum changehint);
	void BindVao() const;
	void SetData(const void * dataptr, size_t datasize);
private:
	void VaoStruct::MallocVbo(size_t vbosize) const;
	void VaoStruct::SetSubData(const void * dataptr, size_t datasize) const;
public:
	GLuint Vao;
	GLuint Vbo;
	GLenum StorageHint;
private:
	size_t VboSize = 0;
};

struct ForwardLightData
{
	bool dlshadow = false;
	bool plshadow = false;
	bool slshadow = false;
	mat4 dwvp = mat4(1.0);
	mat4 swvp = mat4(1.0);
	float plfarplane = 60.0f;
	std::vector<DirectionalLight> dlist;
	std::vector<PointLight> plist;
	std::vector<SpotLight> slist;
};

struct EyeAdaptData
{
	float exposure = 1.0;

	GLuint pbo;
	unsigned int pboframecount = 0;
	float * screencolor;
	std::random_device random;

	float * exposurehistory;
	unsigned int oldesthistory;

	struct ConstEyeAdaptData
	{
		unsigned int MCsamplecount = 1024;
		float exposurememory = 0.1;
		float exposuremax = 5;
		float exposuremin = 0.1;
		float exposurechangethreshold = 0.2;
		unsigned int exposurehistorycount = 2;

		unsigned int pbosampleperiod = 5;
	} constant;
};

class DrawController : public ArrayModel // Implemented Ordered trans render
{
public:
	DrawController(unsigned int w, unsigned int h);/*50M Vertex cost VRAM max*/
	void Draw(DrawContext & context);
	DrawController & operator << (const ArrayModel & rhs) { ArrayModel::operator+=(rhs); return *this; }
	DrawController & operator << (const PositionedArrayModel & rhs);
	bool Clear() { mesh.clear(); trobjlist.clear(); }
	std::unordered_map<int, DirectionalLight> & GetDLight() { return dlist; }
	const std::unordered_map<int, DirectionalLight> & GetDLight() const { return dlist; }
	std::unordered_map<int, PointLight> & GetPLight() { return plist; }
	const std::unordered_map<int, PointLight> & GetPLight() const { return plist; }
	std::unordered_map<int, SpotLight> & GetSLight() { return slist; }
	const std::unordered_map<int, SpotLight> & GetSLight() const { return slist; }
protected:
	std::vector<PositionedArrayModel> trobjlist;

	std::unordered_map<int, DirectionalLight> dlist;
	std::unordered_map<int, PointLight> plist;
	std::unordered_map<int, SpotLight> slist;

	ForwardLightShaderController * flsc;
	HDRShaderController * hdrsc;
	BloomShaderController * bsc;
	MixShaderController * msc;
	DepthShaderController * dsc;

	FboStruct * hdrfboptr;
	FboStruct * pingpongfboptr;
	FboStruct * mixfboptr;
	FboStruct * ddepthfboptr;
	FboStruct * sdepthfboptr;
	FboStruct * pdepthfboptr;
	FboStruct screenfbo;

	VaoStruct * depthvaoptr;
	VaoStruct * forwardvaoptr;
	VaoStruct * quadvaoptr;

	EyeAdaptData eadata;

	void EyeAdaptMC();
	void PrepareQuad();
	void RenderDepthSingleFace(FboStruct * target, mat4 lightwvp, size_t vertsize);
	void RenderDepthCubeFace(FboStruct * target, const mat4 lightview[6], const vec3& lightpos, size_t vertsize, float farplane);
	ForwardLightData RenderShadow(bool isshadow, unsigned int w, unsigned int h, vec3 eye, size_t vertsize);
	float CalculateLightRange(float intensity, Attenuation attenuation, float threshold);
	std::vector<vec3> GetNonTransObjList() const;
private:
	unsigned int width, height;
	unsigned int depthtexwidth = 4096, depthtexheight = 4096;

};