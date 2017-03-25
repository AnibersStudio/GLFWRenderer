#pragma once

#include <vector>

#include "GLCommon.h"
#include "CommonTools.h"

struct FboCompRecord
{
	/// <summary> Color attachment or depth attachment. 
	/// <para> Must be GL_COLOR_ATTACHMENT[0,15] , GL_DEPTH_ATTACHMENT or GL_DEPTH_BUFFER </para>
	/// </summary>
	GLenum attachment;
	/// <summary> Internal format
	/// <para> For color, this is GL_R(G)(B)(A)[8,32] </para>
	/// <para> For depth, this is GL_DEPTH_COMPONENT[8,32] </para>
	/// </summary>
	GLenum format;
	/// <summary> Texture parameter to set. This makes no sense to GL_DEPTH_BUFFER </summary>
	std::vector<std::pair<GLenum, GLenum>> paralist;
	/// <summary> The border color when GL_TEXTURE_WARP is set to CLAMP_TO_BORDER
	/// <para> This makes no sense to GL_DEPTH_BUFFER or WARP isn't set to CLAMP_TO_BORDER </para>
	/// </summary>
	glm::vec4 bordercolor = glm::vec4(1.0);
};

struct FboRecord
{
	/// <summary> Width of the texture </summary>
	unsigned int width;
	/// <summary> Height of the texuture </summary>
	unsigned int height;
	/// <summary> All of the component attached to 
	std::vector<FboCompRecord> component;
	/// <summary> Texture type of the attachments. All of the color & depth attachment should be the same type
	/// <para> Must be one of GL_TEXTURE_2D GL_TEXTURE_3D GL_TEXTURE_CUBEMAP </para>
	/// </summary>
	GLenum texturetype = GL_TEXTURE_2D;
};