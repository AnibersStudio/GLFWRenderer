#pragma comment(lib, "Freeimage.lib")

#include "Texture2D.h"

Texture2D::Texture2D(std::string path, bool colortexture)
{
	auto filetype = FreeImage_GetFileType(path.c_str(), 0);
	if (filetype == FIF_UNKNOWN)
	{
		throw DrawErrorException("FreeImage:" + path, "Doesn't exist or isn't an image file.");
	}

	auto image = FreeImage_Load(filetype, path.c_str(), 0);
	auto blob = new BYTE[FreeImage_GetHeight(image) * FreeImage_GetPitch(image)];
	FreeImage_ConvertToRawBits(blob, image, FreeImage_GetPitch(image), FreeImage_GetBPP(image), FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
	
	bool isSRGB = colortexture;
	bool issmooth = colortexture;//Assuming that smooth image has mipmap
	bool hasalpha;
	if (FreeImage_GetBPP(image) == 32)
	{
		hasalpha = true;
	}
	else if (FreeImage_GetBPP(image) == 24)
	{
		hasalpha = false;
	}
	else
	{
		FreeImage_Unload(image);
		throw DrawErrorException("FreeImage:" + path, "Image bitwidth isn't supported.");
	}
	//os: win-0 linux-1 other-2
	int os = 2;
#ifdef WIN32
	os = 0;
#endif
#ifdef LINUX
	os = 1;
#endif

	//internalformat[isSRGB][hasalpha]
	GLenum internalformats[2][2] =
	{ { GL_RGB, GL_RGBA },
	{GL_SRGB, GL_SRGB_ALPHA} };
	//imageformats[os:win-0 linux-1 other-2][hasalpha]
	GLenum imageformats[3][2] =
	{ {GL_BGR, GL_BGRA},
	{GL_BGR, GL_BGRA},
	{GL_RGB, GL_RGBA} };
	//determine image format
	GLint internalformat = internalformats[isSRGB][hasalpha];
	GLint imageformat = imageformats[os][hasalpha];
	
	glGenTextures(1, &texobj);
	glTextureImage2DEXT(texobj, textarget, 0, internalformat, FreeImage_GetWidth(image), FreeImage_GetHeight(image), 0, imageformat, GL_UNSIGNED_BYTE, blob);
	if (issmooth)
	{
		glGenerateTextureMipmapEXT(texobj, textarget);
	}
	FreeImage_Unload(image);

	//determine filters
	//filters[issmooth][min-0 mag-1]
	GLenum filters[2][2] =
	{ { GL_NEAREST, GL_NEAREST},
	{ GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR} };
	glTextureParameteriEXT(texobj, textarget, GL_TEXTURE_MIN_FILTER, filters[issmooth][0]);
	glTextureParameteriEXT(texobj, textarget, GL_TEXTURE_MAG_FILTER, filters[issmooth][1]);

	texhandle = glGetTextureHandleARB(texobj);
	glMakeTextureHandleResidentARB(texhandle);

	loaded = true;
	texpath = path;
	iscolortexture = colortexture;
}

void Texture2D::SetAF(unsigned int afscale)
{
	if (afscale != af)
	{
		af = afscale;
		glTextureParameterf(texobj, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(af));
	}
}
