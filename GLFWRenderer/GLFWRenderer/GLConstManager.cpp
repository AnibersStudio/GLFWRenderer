#include "GLConstManager.h"

GLConstManager::GLConstManager()
{
	for (auto & n : intconsts)
	{
		RegisterIntv(n, 1);
	}
	for (auto & n : floatconsts)
	{
		RegisterFloatv(n, 1);
	}
}

GLConstManager & GLConstManager::GetInstance()
{
	static GLConstManager instance;
	return instance;
}

const std::vector<int>& GLConstManager::GetIntv(GLenum name)
{
	return inttable[name];
}

const std::vector<float>& GLConstManager::GetFloatv(GLenum name)
{
	return floattable[name];
}

void GLConstManager::RegisterIntv(GLenum name, unsigned int count)
{
	auto & t = inttable[name];
	if (t.size() < count)
		t.resize(count);
	glGetIntegerv(name, &t[0]);
}

void GLConstManager::RegisterFloatv(GLenum name, unsigned int count)
{
	auto & t = floattable[name];
	if (t.size() < count)
		t.resize(count);
	glGetFloatv(name, &t[0]);
}
