#pragma once

#include <unordered_map>
#include <unordered_set>

#include "GLCommon.h"
class GLConstManager
{
public:
	static GLConstManager & GetInstance();
	const std::vector<int>& GetIntv(GLenum name);
	const std::vector<float>& GetFloatv(GLenum name);
	void RegisterIntv(GLenum name, unsigned int count);
	void RegisterFloatv(GLenum name, unsigned int count);
private:
	GLConstManager();

	std::unordered_map<GLenum, std::vector<int>> inttable;
	std::unordered_map<GLenum, std::vector<float>> floattable;
};