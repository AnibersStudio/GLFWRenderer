#include "TextureLoader.h"
TextureLoader & TextureLoader::GetInstance()
{
	static TextureLoader Instance;
	return Instance;
}

const Texture2D * TextureLoader::Load2DTexture(std::string path, bool iscolortexture)
{
	const Texture2D * tex = nullptr;
	try
	{
		tex = texmap.at(std::tuple<std::string, bool>(path, iscolortexture));
	}
	catch (const std::out_of_range&)
	{
		texmap[std::tuple<std::string, bool>(path, iscolortexture)] = tex = new Texture2D(path, iscolortexture);
	}
	return tex;
}