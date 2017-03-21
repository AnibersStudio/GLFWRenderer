#pragma once
#include <string>
#include <sstream>
/// <summary> Convert any numberic type to string </summary>
template<typename T>
std::string tostr(const T& input)
{
	std::stringstream converter;
	std::string str;
	converter << input;
	converter >> str;
	return str;
}
/// <summary> Initialize the value of an array </summary>
template<typename T>
void init(T * array_ptr, const T& init_value, size_t count)
{
	for (size_t i = 0U; i != count; i++)
	{
		array_ptr[i] = init_value;
	}
}

/// <summary> Check GL errors </summary> 
void _CheckGLError(const char* file, int line);
#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

/// <summary> An exception for renderer </summary>
struct DrawErrorException : public std::exception
{
	DrawErrorException(std::string p, std::string e) : path(p), glerror(e) {}
	/// <summary> Path or id of the errored file </summary>
	std::string path;
	/// <summary> Error that occurs </summary>
	std::string glerror;
	/// <summary> Get the excaption msg </summary>
	virtual const char * what() const override {
		std::string msg(path + "::" + glerror);
		char * content = new char[msg.size() + 1];
		for (unsigned int i = 0; i != msg.size(); i++)
		{
			content[i] = msg[i];
		}
		content[msg.size()] = '\0';
		return content;
	}
};