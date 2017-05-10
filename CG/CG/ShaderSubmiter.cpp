#include "ShaderSubmiter.h"
#include <fstream>
void ShaderSubmiter::AddShader(GLenum shadertype, const char * shadertext)
{
	GLuint shader = glCreateShader(shadertype);
	glShaderSource(shader, 1, &shadertext, NULL);
	glCompileShader(shader);
	GLint compilestate;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compilestate);
	if (compilestate)
	{
		shaderobjlist.push_back(shader);
	}
	else
	{
		GLint loglength;
		GLchar * logadd;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglength);
		logadd = new GLchar[loglength + 1];
		glGetShaderInfoLog(shader, loglength, &loglength, logadd);
		{//编译出错，做善后处理
			DrawErrorException exp("ShaderController:UnknowFile", logadd);
			delete[] logadd;
			throw exp;
		}
	}
}

void ShaderSubmiter::LinkShader()
{
	shaderprogram = glCreateProgram();
	for (auto soit = shaderobjlist.begin(); soit != shaderobjlist.end(); soit++)
	{
		glAttachShader(shaderprogram, *soit);
	}
	glLinkProgram(shaderprogram);
	GLint linked;
	glGetProgramiv(shaderprogram, GL_LINK_STATUS, &linked);
	if (linked)
	{
		for (auto soit : shaderobjlist)
		{
			glDeleteShader(soit);
		}
	}
	else
	{
		GLint loglength;
		GLchar * logadd;
		glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &loglength);
		logadd = new GLchar[loglength + 1];
		glGetProgramInfoLog(shaderprogram, loglength, &loglength, logadd);
		{//渲染程序链接出错，做善后处理
			DrawErrorException exp("ShaderController:UnknowFile", logadd);
			delete[] logadd;
			throw exp;
		}
	}
}

GLuint ShaderSubmiter::GetUniformLocation(const std::string & uniformname) const
{
	GLuint uniformloc = glGetUniformLocation(shaderprogram, uniformname.c_str());
	if (uniformloc == 0xFFFFFFFF)
		throw DrawErrorException{"ShaderController:ShaderProgram" + tostr(shaderprogram) + ":" + uniformname, "Error getting location of uniform. Maybe optimized out by compiler."};
	return uniformloc;
}

