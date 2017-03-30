#pragma once
#include <string>
#include <unordered_map>

#include "Dependencies/include/FreeImage.h"

#include "GLCommon.h"
#include "CommonTools.h"

class Texture2D
{
public:
	/// <summary> Create a default Texture2D which is not loaded. </summary>
	Texture2D() = default;
	/// <summary> Create a GL_TEXTURE_2D  who is a color texture or a value texture
	/// <para> Throws DrawErrorException if file not exist or not recognized </para>
	/// </summary>
	Texture2D(std::string path, bool colortexture);

	/// <summary> Set anisotropic filter scale. Must be greater than 1 which means off </summary>
	void SetAF(unsigned int afscale);

	const std::string & GetPath() const { return texpath; }
	GLuint GetObjectID() const { return texobj; }
	GLuint64 GetObjectHandle() const { return texhandle; }
	bool GetIsColorTex() const { return iscolortexture; }
	bool operator ==(const Texture2D & rhs) const { return texpath == rhs.texpath && loaded == rhs.loaded && textarget == rhs.textarget && texobj == rhs.texobj && iscolortexture == rhs.iscolortexture; };
	bool operator != (const Texture2D & rhs) const { return !operator==(rhs); }
	operator bool()const { return loaded; };

private:
	std::string texpath = "";
	bool loaded = false;
	GLenum textarget = GL_TEXTURE_2D;
	GLuint texobj = 0xFFFFFFFF;
	GLuint64 texhandle = 0xFFFFFFFF;
	/// <summary> colortexture is min-and-mag smooth and has mipmap, but non-colortexture the opposite </summary>
	bool iscolortexture;
	unsigned int af;
};

namespace std
{
	template <>
	struct hash <Texture2D>
	{
		std::size_t operator()(const Texture2D & obj) const
		{
			return std::hash<std::string>()(obj.GetPath()) ^ std::hash<bool>()(obj.GetIsColorTex());
		}
	};
}

namespace std
{
	template <>
	struct hash <const std::tuple<std::string, bool>>
	{
		std::size_t operator()(const std::tuple<std::string, bool> obj) const
		{
			return std::hash<std::string>()(std::get<0>(obj)) ^ std::hash<bool>()(std::get<1>(obj));
		}
	};
}

