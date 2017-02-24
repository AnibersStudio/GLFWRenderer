#pragma once
#include <string>
#include <sstream>
template<typename T>
std::string tostr(const T& input)
{
	std::stringstream converter;
	std::string str;
	converter << input;
	converter >> str;
	return str;
}

template<typename T>
void init(T * array_ptr, const T& init_value, size_t count)
{
	for (size_t i = 0U; i != count; i++)
	{
		array_ptr[i] = init_value;
	}
}

void _CheckGLError(const char* file, int line);
#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

