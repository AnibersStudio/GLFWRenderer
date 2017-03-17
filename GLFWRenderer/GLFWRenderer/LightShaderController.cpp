#include "LightShaderController.h"
#include "CommonTools.h"
#include <fstream>





bool ForwardLightShaderController::LoadShaderPipeline()
{
	std::string vers, fras;
	std::string tmp;
	std::ifstream versin(vertexpath), fragin(fragmentpath);
	while (std::getline(versin, tmp))
	{
		vers += tmp;
		vers += "\n";
	}
	tmp.clear();
	while (std::getline(fragin, tmp))
	{
		fras += tmp;
		fras += "\n";
	}
	AddShader(GL_VERTEX_SHADER, vers.c_str());
	AddShader(GL_FRAGMENT_SHADER, fras.c_str());
	LinkShader();

	GetAllUniforms();
	
	return true;
}

void ForwardLightShaderController::Use()
{
	ShaderController::Use();
	SetSafeState();
}

void ForwardLightShaderController::SetMatrix(const glm::mat4 & W, const glm::mat4 & V, const glm::mat4 & P) const
{
	glm::mat4 WVP = P * V * W;
	glUniformMatrix4fv(uniformgroup.WVPloc, 1, GL_FALSE, &WVP[0][0]);
}

void ForwardLightShaderController::SetLightWVP(const glm::mat4 & dlwvp, const glm::mat4 & slwvp, float plfarplane)
{
	glUniformMatrix4fv(uniformgroup.dllightWVPloc, 1, GL_FALSE, &dlwvp[0][0]);
	glUniformMatrix4fv(uniformgroup.sllightWVPloc, 1, GL_FALSE, &slwvp[0][0]);
	glUniform1f(uniformgroup.plfarplaneloc, plfarplane);
}

void ForwardLightShaderController::SetTextureDepth(GLuint dt, GLuint pt, GLuint st)
{
	if (dt)
	{
		glActiveTexture(GL_TEXTURE0 + 10);
		glBindTexture(GL_TEXTURE_2D, dt);
		glUniform1i(uniformgroup.isdldepthloc, 1);
	}
	if (pt)
	{
		glActiveTexture(GL_TEXTURE0 + 11);
		glBindTexture(GL_TEXTURE_CUBE_MAP, pt);
		glUniform1i(uniformgroup.ispldepthloc, 1);
	}
	if (st)
	{
		glActiveTexture(GL_TEXTURE0 + 12);
		glBindTexture(GL_TEXTURE_2D, st);
		glUniform1i(uniformgroup.issldepthloc, 1);
	}
}

void ForwardLightShaderController::SetEye(const glm::vec3 eyepos) const
{
	glUniform3fv(uniformgroup.Eyeloc, 1, &eyepos[0]);
}

void ForwardLightShaderController::SetBloom(bool isbloom) const
{
	glUniform1i(uniformgroup.isbloomloc, isbloom);
}

void ForwardLightShaderController::SetDLight(unsigned int count, DirectionalLight * dl)
{
	glUniform1ui(uniformgroup.dlcountloc, count);
	for (int i = 0; i != count && i != MAXDIRECTIONALLIGHT; i++)
	{
		const DirectionalLightLoc & dlloc = uniformgroup.dlloc[i];
		glUniform1f(dlloc.intensity, dl[i].intensity);
		glUniform3fv(dlloc.color, 1, &dl[i].color[0]);
		glUniform1f(dlloc.diffuse, dl[i].diffuse);
		glUniform1f(dlloc.specular, dl[i].specular);
		glUniform3fv(dlloc.direction, 1, &dl[i].direction[0]);

	}
}

void ForwardLightShaderController::SetPLight(unsigned int count, PointLight * pl)
{
	glNamedBufferDataEXT(pointubo, count * sizeof(PointLight), pl,  GL_STREAM_DRAW);
}

void ForwardLightShaderController::SetSLight(unsigned int count, SpotLight * sl)
{
	glNamedBufferDataEXT(spotubo, count * sizeof(SpotLight), sl, GL_STREAM_DRAW);
}

void ForwardLightShaderController::SetTexMaterial(const TexturedMaterial texmat) const
{
	SetMaterial(texmat);
	SetNoneTexture();
	if (texmat.diffusetex)
		SetTextureDiffuse(*texmat.diffusetex);
	if (texmat.speculartex)
		SetTextureSpecular(*texmat.speculartex);
	if (texmat.normaltex)
		SetTextureNormal(*texmat.normaltex);
	if (texmat.emissivetex)
		SetTextureEmissive(*texmat.emissivetex);
	if (texmat.transtex)
		SetTextureTrans(*texmat.transtex);

}

void ForwardLightShaderController::SetMaterial(const Material & m) const
{
	glUniform3fv(uniformgroup.materialloc.ambientcolor, 1, &m.ambientcolor[0]);
	glUniform3fv(uniformgroup.materialloc.diffusecolor, 1, &m.diffusecolor[0]);
	glUniform3fv(uniformgroup.materialloc.specularcolor, 1, &m.specularcolor[0]);
	glUniform3fv(uniformgroup.materialloc.emissivecolor, 1, &m.emissivecolor[0]);
	glUniform1f(uniformgroup.materialloc.shininess, m.shininess);
	glUniform1f(uniformgroup.materialloc.trans, m.transparency);
}

void ForwardLightShaderController::SetTextureDiffuse(const Texture2D & d)  const
{
	d.Bind(0);
	glUniform1i(uniformgroup.isdiffuseloc, 1);
}

void ForwardLightShaderController::SetTextureSpecular(const Texture2D & s)  const
{
	s.Bind(1);
	glUniform1i(uniformgroup.isspecularloc, 1);
}

void ForwardLightShaderController::SetTextureEmissive(const Texture2D & e)  const
{
	e.Bind(2);
	glUniform1i(uniformgroup.isemissiveloc, 1);
}

void ForwardLightShaderController::SetTextureNormal(const Texture2D & n) const
{
	n.Bind(3);
	glUniform1i(uniformgroup.isnormalloc, 1);
}

void ForwardLightShaderController::SetTextureTrans(const Texture2D & t) const
{
	t.Bind(4);
	glUniform1i(uniformgroup.istransloc, 1);
}

void ForwardLightShaderController::GetAllUniforms() 
{
	uniformgroup.isdiffuseloc = GetUniformLocation("isdiffuse");
	uniformgroup.diffusesamplerloc = GetUniformLocation("diffusesampler");
	uniformgroup.isspecularloc = GetUniformLocation("isspecular");
	uniformgroup.specularsamplerloc = GetUniformLocation("specularsampler");
	uniformgroup.isemissiveloc = GetUniformLocation("isemissive");
	uniformgroup.emissivesamplerloc = GetUniformLocation("emissivesampler");
	uniformgroup.isnormalloc = GetUniformLocation("isnormal");
	uniformgroup.normalsamplerloc = GetUniformLocation("normalsampler");
	uniformgroup.istransloc = GetUniformLocation("istrans");
	uniformgroup.transsamplerloc = GetUniformLocation("transsampler");
	uniformgroup.WVPloc = GetUniformLocation("WVP");
	uniformgroup.Eyeloc = GetUniformLocation("Eye");
	uniformgroup.dlcountloc = GetUniformLocation("dlcount");
	uniformgroup.isbloomloc = GetUniformLocation("isbloom");
	for (unsigned int i = 0; i != MAXDIRECTIONALLIGHT; i++)
	{
		DirectionalLightLoc& dlloc = uniformgroup.dlloc[i];
		dlloc.intensity = GetUniformLocation("dl[" + tostr(i) + "].bl.intensity");
		dlloc.color = GetUniformLocation("dl[" + tostr(i) + "].bl.color");
		dlloc.diffuse = GetUniformLocation("dl[" + tostr(i) + "].bl.diffuse");
		dlloc.specular = GetUniformLocation("dl[" + tostr(i) + "].bl.specular");
		dlloc.direction = GetUniformLocation("dl[" + tostr(i) + "].direction");
	}
	uniformgroup.materialloc.ambientcolor = GetUniformLocation("material.ambientcolor");
	uniformgroup.materialloc.diffusecolor = GetUniformLocation("material.diffusecolor");
	uniformgroup.materialloc.specularcolor = GetUniformLocation("material.specularcolor");
	uniformgroup.materialloc.emissivecolor = GetUniformLocation("material.emissivecolor");
	uniformgroup.materialloc.shininess = GetUniformLocation("material.shininess");
	uniformgroup.materialloc.trans = GetUniformLocation("material.trans");

	uniformgroup.dllightWVPloc = GetUniformLocation("dlLightWVP");
	uniformgroup.sllightWVPloc = GetUniformLocation("slLightWVP");
	uniformgroup.plfarplaneloc = GetUniformLocation("plfarplane");

	uniformgroup.isdldepthloc = GetUniformLocation("isdldepth");
	uniformgroup.dldepthsamplerloc = GetUniformLocation("dldepthsampler");
	uniformgroup.ispldepthloc = GetUniformLocation("ispldepth");
	uniformgroup.pldepthsamplerloc = GetUniformLocation("pldepthsampler");
	uniformgroup.issldepthloc = GetUniformLocation("issldepth");
	uniformgroup.sldepthsamplerloc = GetUniformLocation("sldepthsampler");
}

void ForwardLightShaderController::SetupUniformBuffer()
{
	glGenBuffers(1, &pointubo);
	glGenBuffers(1, &spotubo);
	pointuniformindex = glGetUniformBlockIndex(shaderprogram, "pointlight");
	spotuniformindex = glGetUniformBlockIndex(shaderprogram, "spotlight");
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, pointubo);
	glUniformBlockBinding(shaderprogram, pointuniformindex, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, spotubo);
	glUniformBlockBinding(shaderprogram, spotuniformindex, 2);
}

void ForwardLightShaderController::SetSafeState()
{
	glUniform1ui(uniformgroup.dlcountloc, 0);
	glUniform1ui(uniformgroup.isdiffuseloc, 0);
	glUniform1ui(uniformgroup.isspecularloc, 0);
	glUniform1ui(uniformgroup.isemissiveloc, 0);
	glUniform1ui(uniformgroup.isnormalloc, 0);
	glUniform1ui(uniformgroup.istransloc, 0);
	glUniform1i(uniformgroup.diffusesamplerloc, 0);
	glUniform1i(uniformgroup.specularsamplerloc, 1);
	glUniform1i(uniformgroup.emissivesamplerloc, 2);
	glUniform1i(uniformgroup.normalsamplerloc, 3);
	glUniform1i(uniformgroup.transsamplerloc, 4);

	glUniform1ui(uniformgroup.isdldepthloc, 0);
	glUniform1ui(uniformgroup.ispldepthloc, 0);
	glUniform1ui(uniformgroup.issldepthloc, 0);
	glUniform1i(uniformgroup.dldepthsamplerloc, 10);
	glUniform1i(uniformgroup.pldepthsamplerloc, 11);
	glUniform1i(uniformgroup.sldepthsamplerloc, 12);

	SetLightWVP(mat4(1.0), mat4(1.0), 60.0f);
}

void ForwardLightShaderController::SetNoneTexture() const
{
	glUniform1ui(uniformgroup.isdiffuseloc, 0);
	glUniform1ui(uniformgroup.isspecularloc, 0);
	glUniform1ui(uniformgroup.isemissiveloc, 0);
	glUniform1ui(uniformgroup.isnormalloc, 0);
	glUniform1ui(uniformgroup.istransloc, 0);
}


bool HDRShaderController::LoadShaderPipeline()
{
	std::string vers, fras;
	std::string tmp;
	std::ifstream versin(vertexpath), fragin(fragmentpath);
	while (std::getline(versin, tmp))
	{
		vers += tmp;
		vers += "\n";
	}
	tmp.clear();
	while (std::getline(fragin, tmp))
	{
		fras += tmp;
		fras += "\n";
	}
	AddShader(GL_VERTEX_SHADER, vers.c_str());
	AddShader(GL_FRAGMENT_SHADER, fras.c_str());
	LinkShader();

	GetAllUniforms();

	return true;
}

void HDRShaderController::SetGamma(float g)
{
	glUniform1f(uniformgroup.gammaloc, g);
}

void HDRShaderController::SetScreenTexture(GLuint buffer)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer);
}

void HDRShaderController::SetIsHDR(bool ishdr)
{
	glUniform1ui(uniformgroup.isHDRloc, ishdr);
}

void HDRShaderController::GetAllUniforms()
{
	uniformgroup.gammaloc = GetUniformLocation("gamma");
	uniformgroup.isHDRloc = GetUniformLocation("isHDR");
	uniformgroup.screenimageloc = GetUniformLocation("screenimagesampler");
	uniformgroup.exposureloc = GetUniformLocation("exposure");
}

void HDRShaderController::SetSafeState()
{
	glUniform1f(uniformgroup.gammaloc, 2.2f);
	glUniform1ui(uniformgroup.isHDRloc, false);
	glUniform1i(uniformgroup.screenimageloc, 0);
}

void HDRShaderController::SetExposure(float exposure)
{
	glUniform1f(uniformgroup.exposureloc, exposure);
}

void HDRShaderController::Use()
{
	ShaderController::Use();
	SetSafeState();
}


bool BloomShaderController::LoadShaderPipeline()
{
	std::string vers, fras;
	std::string tmp;
	std::ifstream versin(vertexpath), fragin(fragmentpath);
	while (std::getline(versin, tmp))
	{
		vers += tmp;
		vers += "\n";
	}
	tmp.clear();
	while (std::getline(fragin, tmp))
	{
		fras += tmp;
		fras += "\n";
	}
	AddShader(GL_VERTEX_SHADER, vers.c_str());
	AddShader(GL_FRAGMENT_SHADER, fras.c_str());
	LinkShader();

	GetAllUniforms();

	return true;
}

void BloomShaderController::SetImageTexture(GLuint blurbuffer)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurbuffer);
}

void BloomShaderController::SetBlurDirection(bool ishorizonal)
{
	glUniform1ui(uniformgroup.ishorizonalloc, ishorizonal);
}

void BloomShaderController::GetAllUniforms()
{
	uniformgroup.ishorizonalloc = GetUniformLocation("ishorizonal");
	uniformgroup.imagesamplerloc = GetUniformLocation("imagesampler");
}

void BloomShaderController::SetSafeState()
{
	glUniform1ui(uniformgroup.ishorizonalloc, true);
	glUniform1i(uniformgroup.imagesamplerloc, 0);
}

void BloomShaderController::Use()
{
	ShaderController::Use();
	SetSafeState();
}

bool MixShaderController::LoadShaderPipeline()
{
	std::string vers, fras;
	std::string tmp;
	std::ifstream versin(vertexpath), fragin(fragmentpath);
	while (std::getline(versin, tmp))
	{
		vers += tmp;
		vers += "\n";
	}
	tmp.clear();
	while (std::getline(fragin, tmp))
	{
		fras += tmp;
		fras += "\n";
	}
	AddShader(GL_VERTEX_SHADER, vers.c_str());
	AddShader(GL_FRAGMENT_SHADER, fras.c_str());
	LinkShader();

	GetAllUniforms();

	return true;
}

void MixShaderController::SetColorTexture(GLuint buffer)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer);
}

void MixShaderController::SetBrightColorTexture(GLuint buffer)
{
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, buffer);
}

void MixShaderController::GetAllUniforms()
{
	uniformgroup.colorsamplerloc = GetUniformLocation("colorsampler");
	uniformgroup.brightsamplerloc = GetUniformLocation("brightsampler");
}

void MixShaderController::SetSafeState()
{
	glUniform1i(uniformgroup.colorsamplerloc, 0);
	glUniform1i(uniformgroup.brightsamplerloc, 1);
}

void MixShaderController::Use()
{
	ShaderController::Use();
	SetSafeState();
}

bool DepthShaderController::LoadShaderPipeline()
{
	std::string vers, fras;
	std::string tmp;
	std::ifstream versin(vertexpath), fragin(fragmentpath);
	while (std::getline(versin, tmp))
	{
		vers += tmp;
		vers += "\n";
	}
	tmp.clear();
	while (std::getline(fragin, tmp))
	{
		fras += tmp;
		fras += "\n";
	}
	AddShader(GL_VERTEX_SHADER, vers.c_str());
	AddShader(GL_FRAGMENT_SHADER, fras.c_str());
	LinkShader();

	GetAllUniforms();

	return true;
}

void DepthShaderController::SetLightWVP(const mat4 & wvp)
{
	glUniformMatrix4fv(uniformgroup.LightWVPloc, 1, GL_FALSE, &wvp[0][0]);
}

void DepthShaderController::Use()
{
	ShaderController::Use();
	SetSafeState();
}

void DepthShaderController::GetAllUniforms()
{
	uniformgroup.LightWVPloc = GetUniformLocation("LightWVP");
	uniformgroup.islinearloc = GetUniformLocation("islinear");
	uniformgroup.lightposloc = GetUniformLocation("lightpos");
	uniformgroup.farplaneloc = GetUniformLocation("farplane");
}

void DepthShaderController::SetSafeState()
{
	glUniform1i(uniformgroup.islinearloc, 0);
}

void DepthShaderController::SetLinearDepth(const vec3 & lightpos, float farplane)
{
	glUniform1i(uniformgroup.islinearloc, 1);
	glUniform3fv(uniformgroup.lightposloc, 1, &lightpos[0]);
	glUniform1f(uniformgroup.farplaneloc, farplane);
}

