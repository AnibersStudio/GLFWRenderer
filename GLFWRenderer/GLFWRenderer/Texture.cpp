#pragma comment(lib, "Freeimage.lib")

#include "Texture.h"
#include <iostream>
Texture2D::Texture2D(GLenum ttarget, std::string path, bool issRGB, bool magnifysmooth, bool mipmaped)
{
	textarget = ttarget;
	auto image = FreeImage_Load(FreeImage_GetFileType(path.c_str(), 0), path.c_str(), 0);
	auto blob = new BYTE[FreeImage_GetHeight(image) * FreeImage_GetPitch(image)];
	FreeImage_ConvertToRawBits(blob, image, FreeImage_GetPitch(image), FreeImage_GetBPP(image), FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
	GLint internalformat;
	GLint imageformat;
	unsigned int bitwidth = FreeImage_GetBPP(image);
	if (FreeImage_GetBPP(image) == 32)
	{
		if (issRGB)
		{
			internalformat = GL_SRGB_ALPHA;
		}
		else
		{
			internalformat = GL_RGBA;
		}
		imageformat = internalformat;
#ifdef WIN32
		imageformat = GL_BGRA;
#endif
#ifdef LINUX
		imageformat = GL_BGRA;
#endif
	}
	else if (FreeImage_GetBPP(image) == 24)
	{
		if (issRGB)
		{
			internalformat = GL_SRGB;
		}
		else
		{
			internalformat = GL_RGB;
		}
		imageformat = internalformat;
#ifdef WIN32
		imageformat = GL_BGR;
#endif
#ifdef LINUX
		imageformat = GL_BGR;
#endif
	}
	else
	{
		std::cerr << "Texture2D: Image bitwidth cannot be recognized." << std::endl;
		FreeImage_Unload(image);
		return;
	}
	glGenTextures(1, &texobj);
	glBindTexture(textarget, texobj);
	glTexImage2D(textarget, 0, internalformat, FreeImage_GetWidth(image), FreeImage_GetHeight(image), 0, imageformat, GL_UNSIGNED_BYTE, blob);
	if (mipmaped)
	{
		glGenerateMipmap(textarget);
	}

	FreeImage_Unload(image);
	glBindTexture(textarget, 0);
	loaded = true;
	texpath = path;
	ismagnifysmooth = magnifysmooth;
	havemipmap = mipmaped;
}

bool Texture2D::Bind(unsigned int num) const
{
	if (loaded)
	{
		glActiveTexture(GL_TEXTURE0 + num);
		glBindTexture(textarget, texobj);
		if (havemipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//solve the distant blink
		}
		if (ismagnifysmooth)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	return loaded;
}
