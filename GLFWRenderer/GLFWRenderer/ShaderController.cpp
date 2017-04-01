#include <fstream>
#include "ShaderController.h"
#include "BufferObjectSubmiter.h"
ShaderController::ShaderController(std::vector<std::pair<std::string, GLenum>> shaderlist, std::vector<ShaderVarRec> variablelist)
{
	ConstructShader(shaderlist);
	ConstructVarMap(variablelist);
	GetAllUniformLocation();
}

void ShaderController::Clear()
{
	for (auto & v : varmap)
	{
		if (!v.second.safevalue.empty())
		{
			v.second.isset = false;
		}
	}
}

void ShaderController::Set(std::string name, boost::any value)
{
	try
	{
		ShaderVarRec & varrecord = varmap.at(name);

		switch (varrecord.type)
		{
		case GL_INT:
			glUniform1i(varrecord.location, boost::any_cast<GLint>(value));
			break;
		case GL_UNSIGNED_INT:
			glUniform1ui(varrecord.location, boost::any_cast<GLuint>(value));
			break;
		case GL_FLOAT:
			glUniform1f(varrecord.location, boost::any_cast<GLfloat>(value));
			break;
		case GL_DOUBLE:
			glUniform1d(varrecord.location, boost::any_cast<GLdouble>(value));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(varrecord.location, 1, &(boost::any_cast<glm::vec2>(value)[0]));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(varrecord.location, 1, &(boost::any_cast<glm::vec3>(value)[0]));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(varrecord.location, 1, &(boost::any_cast<glm::vec4>(value)[0]));
			break;
		case GL_MATRIX3_ARB:
			glUniformMatrix3fv(varrecord.location, 1, GL_FALSE, &(boost::any_cast<glm::mat3>(value)[0][0]));
			break;
		case GL_MATRIX4_ARB:
			glUniformMatrix4fv(varrecord.location, 1, GL_TRUE, &(boost::any_cast<glm::mat4>(value)[0][0]));
			break;
		case GL_TEXTURE_1D:
		case GL_TEXTURE_2D:
		case GL_TEXTURE_3D:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_DEPTH:
			glActiveTexture(GL_TEXTURE0 + varrecord.location);
			glBindTexture(varrecord.type, boost::any_cast<GLuint>(value));
			break;
		case GL_UNIFORM_BUFFER:
			glBindBufferBase(GL_UNIFORM_BUFFER,  varrecord.location, boost::any_cast<GLuint>(value));
			break;
		case GL_SHADER_STORAGE_BUFFER:
			glBindBufferBase(GL_UNIFORM_BUFFER, varrecord.location, boost::any_cast<GLuint>(value));
			break;
		default:
			throw DrawErrorException("LightShaderController:ShaderProgram" + tostr(programid) + ":" + name, "Variable Type not supported.");
			break;
		}
		varrecord.isset = true;
	}
	catch (std::out_of_range)
	{
		throw DrawErrorException("LightShaderController:ShaderProgram" + tostr(programid) + ":" + name, "Variable not exist in table. Add it in constructor.");
	}
	catch (boost::bad_any_cast)
	{
		throw DrawErrorException("LightShaderController:ShaderProgram" + tostr(programid) + ":" + name, "Variable type incompatible with input value. Must be a value of exactly the GLenum.");
	}
}

void ShaderController::Safe()
{
	for (auto & v : varmap)
	{
		if (!v.second.isset && !v.second.safevalue.empty())
		{
			Set(v.second.name, v.second.safevalue);
		}
	}
}

void ShaderController::SetInOneShot(const std::vector<std::pair<std::string, boost::any>> & list)
{
	Clear();
	for (auto & p : list)
	{
		Set(p.first, p.second);
	}
	Safe();
}

void ShaderController::Draw()
{
	Safe();
	Use();
}

void ShaderController::ConstructShader(std::vector<std::pair<std::string, GLenum>> shaderlist)
{
	for (auto shader : shaderlist)
	{
		try {
			AddShader(shader.second, ReadFile(shader.first).c_str());
		}
		catch (DrawErrorException & e)
		{
			e.path = shader.first;
			throw e;
		}
	}
	try {
		LinkShader();
	}
	catch (DrawErrorException & e)
	{
		for (auto shader : shaderlist)
		{
			e.path += shader.first + " ";
		}
		throw e;
	}
	programid = shaderprogram;
}

void ShaderController::ConstructVarMap(std::vector<ShaderVarRec> variablelist)
{
	varmap.reserve(variablelist.size());
	for (auto variable : variablelist)
	{
		variable.location = 0xFFFFFFFF;
		varmap[variable.name] = variable;
	}
}

std::string ShaderController::ReadFile(std::string path)
{
	std::string content;
	std::ifstream in(path);
	std::string tmp;
	while (std::getline(in, tmp))
	{
		content += tmp;
		content += "\n";
	}
	return content;
}

void ShaderController::GetAllUniformLocation()
{
	static unsigned int samplercounter;
	static unsigned int uniformbuffercounter;
	for (auto & v : varmap)
	{
		GLuint samplerlocation;
		GLuint blockindex;
		switch (v.second.type)
		{
		case GL_TEXTURE_1D:
		case GL_TEXTURE_2D:
		case GL_TEXTURE_3D:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_DEPTH:
			samplerlocation = GetUniformLocation(v.second.name);
			glUniform1i(samplerlocation, samplercounter);
			v.second.location = samplercounter;
			samplercounter++;
			break;
		case GL_UNIFORM_BUFFER:
			blockindex = glGetUniformBlockIndex(shaderprogram, v.second.name.c_str());
			glUniformBlockBinding(shaderprogram, blockindex, uniformbuffercounter);
			v.second.location = blockindex;
			uniformbuffercounter++;
			break;
		case GL_SHADER_STORAGE_BUFFER:
			break;
		default:
			v.second.location = GetUniformLocation(v.second.name);
			break;
		}
	}
}
