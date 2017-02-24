#pragma once
#include "GLCommon.h"
#include <string>
#include <FreeImage.h>
class Texture2D
{
public:
	Texture2D() = default;
	Texture2D(GLenum textarget, std::string path, bool issRGB, bool magnifysmooth, bool havemipmap);
	bool Bind(unsigned int num) const;
	const std::string & GetPath() const { return texpath; };
	bool GetisSmooth() const { return ismagnifysmooth; }
	bool MipMaped() const { return havemipmap; }
	bool operator ==(const Texture2D & rhs) const { return texpath == rhs.texpath && ismagnifysmooth == rhs.ismagnifysmooth && havemipmap == rhs.havemipmap; };
	bool operator != (const Texture2D & rhs) const { return !operator==(rhs); }
	operator bool()const { return loaded; };
private:
	std::string texpath = "";
	bool loaded;
	GLenum textarget;
	GLuint texobj;
	bool ismagnifysmooth;
	bool havemipmap;
};

namespace std
{
	template <>
	struct hash <Texture2D>
	{
		std::size_t operator()(const Texture2D & obj) const
		{
			return std::hash<std::string>()(obj.GetPath()) ^ std::hash<bool>()(obj.GetisSmooth()) ^ std::hash<bool>()(obj.MipMaped());
		}
	};
}