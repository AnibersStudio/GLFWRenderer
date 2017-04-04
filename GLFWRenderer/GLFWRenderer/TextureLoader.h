#pragma once
#include "Texture2D.h"

class TextureLoader
{
public:
	static TextureLoader & GetInstance();
	/// <summary> Load a 2D texture from file or directly return who is already opened </summary>
	Texture2D * Load2DTexture(std::string path, bool iscolortexture = true);
private:
	TextureLoader() = default;
	std::unordered_map <const std::tuple<std::string, bool>, Texture2D * > texmap;

};