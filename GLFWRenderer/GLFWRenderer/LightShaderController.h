#pragma once

#include "ShaderController.h"
#include "Texture.h"
#include "Light.h"
#include "Model.h"

#define MAXPOINTLIGHT 512
#define MAXSPOTLIGHT 256
#define MAXDIRECTIONALLIGHT 4

struct LightUniformGroup
{
	GLuint isdiffuseloc;
	GLuint diffusesamplerloc;
	GLuint isspecularloc;
	GLuint specularsamplerloc;
	GLuint isemissiveloc;
	GLuint emissivesamplerloc;
	GLuint isnormalloc;
	GLuint normalsamplerloc;
	GLuint istransloc;
	GLuint transsamplerloc;

	GLuint isdldepthloc;
	GLuint dldepthsamplerloc;
	GLuint ispldepthloc;
	GLuint pldepthsamplerloc;
	GLuint issldepthloc;
	GLuint sldepthsamplerloc;

	GLuint dllightWVPloc;
	GLuint sllightWVPloc;
	GLuint plfarplaneloc;

	GLuint WVPloc;
	GLuint Eyeloc;

	GLuint isbloomloc;

	DirectionalLightLoc dlloc[MAXDIRECTIONALLIGHT];
	GLuint dlcountloc;

	MaterialLocation materialloc;
};

class ForwardLightShaderController : public ShaderController
{
public:
	ForwardLightShaderController(const std::string & v, const std::string & f) : vertexpath(v), fragmentpath(f) {}
	bool LoadShaderPipeline();

	void Use();
	void SetMatrix(const glm::mat4 & W, const glm::mat4 & V, const glm::mat4 & P) const;
	void SetLightWVP(const glm::mat4 & dlwvp, const glm::mat4 & slwvp, float plfarplane);
	void SetTextureDepth(GLuint dt, GLuint pt, GLuint st);
	void SetEye(const glm::vec3 eyepos) const;
	void SetBloom(bool isbloom) const;
	void SetDLight(unsigned int count, DirectionalLight * dl);
	void SetPLight(unsigned int count, PointLight * pl);
	void SetSLight(unsigned int count, SpotLight * sl);
	void SetTexMaterial(const TexturedMaterial texmat) const;
	void SetMaterial(const Material & m) const;
	void SetTextureDiffuse(const Texture2D & d) const;
	void SetTextureSpecular(const Texture2D & s) const;
	void SetTextureEmissive(const Texture2D & e) const;
	void SetTextureNormal(const Texture2D & n) const;
	void SetTextureTrans(const Texture2D & t) const;
	unsigned int GetShaderObj() { return shaderprogram; }
	void SetSafeState();
	void SetNoneTexture() const;
protected:
	void GetAllUniforms();
	void SetupUniformBuffer();
	GLuint pointubo;
	GLuint spotubo;
	GLuint pointuniformindex;
	GLuint spotuniformindex;
private:
	std::string vertexpath;
	std::string fragmentpath;
	LightUniformGroup uniformgroup;
};

struct HDRUniformGroup
{
	GLuint screenimageloc;
	GLuint gammaloc;
	GLuint isHDRloc;
	GLuint exposureloc;
};

class HDRShaderController : public ShaderController
{
public:
	HDRShaderController(const std::string & v, const std::string & f) : vertexpath(v), fragmentpath(f) {}
	bool LoadShaderPipeline();
	void SetGamma(float g);
	void SetScreenTexture(GLuint buffer);
	void SetIsHDR(bool ishdr);
	void SetExposure(float exposure);
	void SetSafeState();
	void Use();
protected:
	void GetAllUniforms();
private:
	std::string vertexpath;
	std::string fragmentpath;
	HDRUniformGroup uniformgroup;
};

struct BloomUniformGroup
{
	GLuint ishorizonalloc;
	GLuint imagesamplerloc;
};

class BloomShaderController : public ShaderController
{
public:
	BloomShaderController(const std::string & v, const std::string & f) : vertexpath(v), fragmentpath(f) {}
	bool LoadShaderPipeline();
	void SetImageTexture(GLuint blurbuffer);
	void SetBlurDirection(bool ishorizonal);
	void SetSafeState();
	void Use();
protected:
	void GetAllUniforms();
private:
	std::string vertexpath;
	std::string fragmentpath;
	BloomUniformGroup uniformgroup;
};

struct MixUniformGroup
{
	GLuint colorsamplerloc;
	GLuint brightsamplerloc;
};

class MixShaderController : public ShaderController
{
public:
	MixShaderController(const std::string & v, const std::string & f) : vertexpath(v), fragmentpath(f) {}
	bool LoadShaderPipeline();
	void SetColorTexture(GLuint buffer);
	void SetBrightColorTexture(GLuint buffer);
	void SetSafeState();
	void Use();
protected:
	void GetAllUniforms();
private:
	std::string vertexpath;
	std::string fragmentpath;
	MixUniformGroup uniformgroup;
};

struct DepthUniformGroup
{
	GLuint LightWVPloc;
	GLuint islinearloc;
	GLuint lightposloc;
	GLuint farplaneloc;
};

class DepthShaderController : public ShaderController
{
public:
	DepthShaderController(const std::string & v, const std::string & f) : vertexpath(v), fragmentpath(f) {}
	bool LoadShaderPipeline();
	void SetLightWVP(const mat4& mvp);
	void Use();
	void SetSafeState();
	void SetLinearDepth(const vec3& lightpos, float farplane);
protected:
	void GetAllUniforms();

private:
	std::string vertexpath;
	std::string fragmentpath;
	DepthUniformGroup uniformgroup;
};