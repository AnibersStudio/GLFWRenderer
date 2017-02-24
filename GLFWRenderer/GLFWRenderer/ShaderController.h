#pragma once
#include "GLCommon.h"
#include "CommonTools.h"
#include <string>
#include <vector>
#include <iostream>
class ShaderController
{
public:
	bool Use() const
	{
		if (compiled)
		{
			glUseProgram(shaderprogram);
		}
		return compiled;
	};
protected:
	bool AddShader(GLenum shadertype, const char * shadertext);
	bool LinkShader();
	GLuint GetUniformLocation(const std::string & uniformname) const;
	GLuint shaderprogram;
private:
	bool compiled = false;
	std::vector<GLuint> shaderobjlist;
};