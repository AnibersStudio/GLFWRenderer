#pragma once

#include <vector>

#include "GLCommon.h"
#include "CommonTools.h"

struct FboCompRecord
{
	/// <summary> Color attachment or depth attachment. 
	/// <para> Must be GL_COLOR_ATTACHMENT[0,15]_EXT , GL_DEPTH_ATTACHMENT_EXT or GL_DEPTH_BUFFER </para>
	/// </summary>
	GLenum attachment;
	/// <summary> Format, aka which component to store in the texture
	/// <para> For color, this is GL_R(G)(B)(A) </para>
	/// <para> For depth, this is GL_DEPTH_COMPONENT </para>
	/// </summary>
	GLenum format;
	/// <summary> Internalformat, aka the bitwise of each component of color.
	/// <para> For color, this is GL_R(G)(B)(A)[8,32] </para>
	/// <para> For depth, this is GL_DEPTH_COMPONENT[8,32] </para>
	/// </summary>
	GLenum internalformat;
	/// <summary> Valuetype, ie. GL_FLOAT </summary>
	GLenum valuetype;
	/// <summary> Texture parameter to set. This makes no sense to GL_DEPTH_BUFFER </summary>
	std::vector<std::pair<GLenum, GLenum>> paralist;
	/// <summary> The border color when GL_TEXTURE_WARP is set to CLAMP_TO_BORDER
	/// <para> This makes no sense to GL_DEPTH_BUFFER or WARP isn't set to CLAMP_TO_BORDER </para>
	/// </summary>
	glm::vec4 bordercolor = glm::vec4(1.0);
};

class Fbo
{
public:
	/// <summary> Generate a FBO whose attachment settings are component and whose texture is of type texturetype.
	/// <para> texturetype must be one of TEXTURE_2D, TEXTURE_3D or TEXTURE_CUBEMAP </para>
	/// <para> For 2D or Cubemap, dimension must contain width and height. For 3D, depth is also needed. </para>
	/// <para> Throws DrawErrorException if texturetype/attachment/format/paralist is not supported or multiple depth buffer</para>
	/// </summary>
	Fbo(std::vector<unsigned int> dimension, std::vector<FboCompRecord> comp, GLenum textype = GL_TEXTURE_2D);
	/// <summary> Bind FBO as a render target </summary>
	void Bind();
	/// <summary> Bind FBO as a render target, only write to depth attachment </summary>
	void BindDepth();


	GLuint GetFboID() const { return fbo; }
	const std::vector<GLuint> & GetColorID() const { return colorattachment; }
	/// <summary> Returns 0xFFFFFFFF if not exist </summary>
	GLuint GetDepthID() const { return depthattachment; }

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }
	unsigned int GetDepth() const { return depth; }
	const std::vector<FboCompRecord> & GetComponent() const { return component; }
	GLenum GetTexType() const { return texturetype; }

private:
	GLuint Fbo::GenerateTexture(GLenum internalformat, GLenum format, GLenum valuetype, std::vector<std::pair<GLenum, GLenum>> paralist, glm::vec4 bordercolor) const;

	GLuint fbo;
	std::vector<GLuint> colorattachment;
	GLuint depthattachment = 0xFFFFFFFF;

	unsigned int width;
	unsigned int height;
	unsigned int depth = 1;
	std::vector<FboCompRecord> component;
	GLenum texturetype;

	std::vector<GLenum> drawbuffers;
	bool onlydrawdepth = false;
};