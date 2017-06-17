#include "TextureLoader.h"
#include "GLConstManager.h"
TextureLoader & TextureLoader::GetInstance()
{
	static TextureLoader Instance;
	return Instance;
}

Texture2D * TextureLoader::Load2DTexture(std::string path, bool iscolortexture)
{
	Texture2D * tex = nullptr;
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

void TextureLoader::SetAF(bool isaf)
{
	if (isaf != af)
	{
		if (isaf)
		{
			for (auto & t : texmap)
			{
				t.second->SetAF(GLConstManager::GetInstance().GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)[0]);
			}
		}
		else
		{
			for (auto & t : texmap)
			{
				t.second->SetAF(1.0f);
			}
		}
	}
}
