#include "RenderFbo.h"

Fbo::Fbo(std::vector<unsigned int> dimension, std::vector<FboCompRecord> comp, GLenum textype) : width(dimension[0]), height(dimension[1]), component(comp), texturetype(textype)
{
	if (texturetype == GL_TEXTURE_3D)
	{
		depth = dimension[2];
	}
	else if (texturetype != GL_TEXTURE_2D && texturetype != GL_TEXTURE_CUBE_MAP)
	{
		throw DrawErrorException("FBO", "Texture type " + tostr(texturetype) + " unsupported.");
	}

	int depthcount = 0;
	drawbuffers.reserve(component.size());
	for (auto c : component)
	{
		if (c.attachment >= GL_COLOR_ATTACHMENT0_EXT && c.attachment <= GL_COLOR_ATTACHMENT15_EXT)
		{
			drawbuffers.push_back(c.attachment);
		}
		else if (c.attachment == GL_DEPTH_ATTACHMENT_EXT || c.attachment == GL_DEPTH_BUFFER)
		{
			depthcount++;
		}
		else
		{
			throw DrawErrorException("FBO", "Attachment type " + tostr(c.attachment) + " unsupported.");
		}
	}
	if (depthcount > 1)
	{
		throw DrawErrorException("FBO" + tostr(fbo), "One FBO can only have one depth attachment.");
	}

	glGenFramebuffers(1, &fbo);
	for (auto c : component)
	{
		if (c.attachment == GL_DEPTH_BUFFER)//Depth buffer
		{
			glGenRenderbuffersEXT(1, &depthattachment);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthattachment);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, c.format, width, height);
			glNamedFramebufferRenderbufferEXT(fbo, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthattachment);
		}
		else if (c.attachment == GL_DEPTH_ATTACHMENT_EXT)//Depth texture
		{
			depthattachment = GenerateTexture(c.internalformat, c.format, c.valuetype, c.paralist, c.bordercolor);
			glNamedFramebufferTextureEXT(fbo, GL_DEPTH_ATTACHMENT_EXT, depthattachment, 0);
		}
		else//Color texture
		{
			GLuint textureid = GenerateTexture(c.internalformat, c.format, c.valuetype, c.paralist, c.bordercolor);
			colorattachment.push_back(textureid);
			glNamedFramebufferTextureEXT(fbo, c.attachment, textureid, 0);
		}
	}

	if (drawbuffers.size())
	{
		glFramebufferDrawBuffersEXT(fbo, drawbuffers.size(), &drawbuffers[0]);
		onlydrawdepth = false;
	}
	else
	{
		glFramebufferDrawBufferEXT(fbo, GL_NONE);
		onlydrawdepth = true;
	}

	GLenum fbostatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (fbostatus == GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		return;
	}
	std::string errormsg;
	switch (fbostatus)
	{
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		errormsg = "Incomplete attachment.";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		errormsg = "Incomplete dimension.";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		errormsg = "Missing attachment";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		errormsg = "Unsupported.";
		break;
	default:
		errormsg = "Unknown incomplete reason.";
		break;
	}
	throw DrawErrorException("FBO" + tostr(fbo), errormsg);
}

void Fbo::Bind()
{
	if (onlydrawdepth && drawbuffers.size())
	{
		glFramebufferDrawBuffersEXT(fbo, drawbuffers.size(), &drawbuffers[0]);
		onlydrawdepth = false;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
}

void Fbo::BindDepth()
{
	if (!onlydrawdepth && drawbuffers.size())
	{
		glFramebufferDrawBufferEXT(fbo, GL_NONE);
		onlydrawdepth = true;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

}

GLuint64 Fbo::GetDepthHandle()
{
	if (depthhandle == 0xFFFFFFFFFFFFFFFF)
	{
		depthhandle = glGetTextureHandleARB(depthattachment);
		glMakeTextureHandleResidentARB(depthhandle);
	}
	return depthhandle;
}

GLuint64 Fbo::GetDepthImageHanle()
{
	if (depthimagehandle == 0xFFFFFFFFFFFFFFFF)
	{
		depthimagehandle = glGetImageHandleARB(depthattachment, 0, false, 0, GL_R32UI);
		glMakeImageHandleNonResidentARB(depthimagehandle);
	}
	return depthimagehandle;
}

GLuint Fbo::GenerateTexture(GLenum internalformat, GLenum format, GLenum valuetype, std::vector<std::pair<GLenum, GLenum>> paralist, glm::vec4 bordercolor) const
{
	GLuint textureid;
	glGenTextures(1, &textureid);

	switch (texturetype)
	{
	case GL_TEXTURE_2D:
		glTextureImage2DEXT(textureid, GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, valuetype, nullptr);
		break;
	case GL_TEXTURE_CUBE_MAP:
		for (int i = 0; i != 6; i++)
		{
			glTextureImage2DEXT(textureid, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, valuetype, nullptr);
		}
		break;
	case GL_TEXTURE_3D:
		glTextureImage3DEXT(textureid, GL_TEXTURE_3D, 0, internalformat, width, height, depth, 0, format, valuetype, nullptr);
		break;
	default:
		break;
	}

	for (auto c : paralist)
	{
		glTextureParameteriEXT(textureid, texturetype, c.first, c.second);
	}
	glTextureParameterfvEXT(textureid, texturetype, GL_TEXTURE_BORDER_COLOR, &bordercolor[0]);
	return textureid;
}
