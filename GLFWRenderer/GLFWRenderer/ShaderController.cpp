#include "ShaderController.h"

void ShaderController::AddShader(GLenum shadertype, const char * shadertext)
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
			ShaderErrorException exp("UnknowFile", logadd);
			delete[] logadd;
			throw exp;
		}
	}
}

void ShaderController::LinkShader()
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
			ShaderErrorException exp("UnknowFile", logadd);
			delete[] logadd;
			throw exp;
		}
	}
}

GLuint ShaderController::GetUniformLocation(const std::string & uniformname) const
{
	GLuint uniformloc = glGetUniformLocation(shaderprogram, uniformname.c_str());
	if (uniformloc == 0xFFFFFFFF)
		throw ShaderErrorException{"ShaderProgram" + tostr(shaderprogram) + ":" + uniformname, "Error getting location of uniform. Maybe optimized out by compiler."};
	return uniformloc;
}

LightShaderController::LightShaderController(std::initializer_list<std::pair<std::string, GLenum>> shaderlist, std::initializer_list<ShaderVarRec> variablelist)
{
	std::vector<std::pair<std::string, GLenum>> shadervec(shaderlist);
	ConstructShader(shadervec);
	std::vector<ShaderVarRec> variablevec(variablelist);
	ConstructVarMap(variablevec);
	GetAllUniformLocation();
}

LightShaderController::LightShaderController(std::vector<std::pair<std::string, GLenum>> shaderlist, std::vector<ShaderVarRec> variablelist)
{
	ConstructShader(shaderlist);
	ConstructVarMap(variablelist);
	GetAllUniformLocation();
}

void LightShaderController::Clear()
{
	for (auto & v : varmap)
	{
		if (!v.second.safevalue.empty())
		{
			v.second.isset = false;
		}
	}
}

void LightShaderController::Set(std::string name, boost::any value)
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
		default:
			throw ShaderErrorException("ShaderProgram" + tostr(programid) + ":" + name, "Variable Type not supported.");
			break;
		}
		varrecord.isset = true;
	}
	catch (std::out_of_range)
	{
		throw ShaderErrorException("ShaderProgram" + tostr(programid) + ":" + name, "Variable not exist in table. Add it in constructor.");
	}
	catch (boost::bad_any_cast)
	{
		throw ShaderErrorException("ShaderProgram" + tostr(programid) + ":" + name, "Variable type incompatible with input value. Must be a value of exactly the GLenum.");
	}
}

void LightShaderController::Safe()
{
	for (auto & v : varmap)
	{
		if (!v.second.isset && !v.second.safevalue.empty())
		{
			Set(v.second.name, v.second.safevalue);
		}
	}
}

void LightShaderController::SetInOneShot(std::vector<std::pair<std::string, boost::any>> list)
{
	Clear();
	for (auto & p : list)
	{
		Set(p.first, p.second);
	}
	Safe();
}

void LightShaderController::Draw()
{
	Safe();
	Use();
}

void LightShaderController::ConstructShader(std::vector<std::pair<std::string, GLenum>> shaderlist)
{
	for (auto shader : shaderlist)
	{
		try {
			AddShader(shader.second, ReadFile(shader.first).c_str());
		}
		catch (ShaderErrorException & e)
		{
			e.path = shader.first;
			throw e;
		}
	}
	try {
		LinkShader();
	}
	catch (ShaderErrorException & e)
	{
		for (auto shader : shaderlist)
		{
			e.path += shader.first + " ";
		}
		throw e;
	}
	programid = shaderprogram;
}

void LightShaderController::ConstructVarMap(std::vector<ShaderVarRec> variablelist)
{
	for (auto variable : variablelist)
	{
		variable.location = 0xFFFFFFFF;
		varmap[variable.name] = variable;
	}
}

std::string LightShaderController::ReadFile(std::string path)
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

void LightShaderController::GetAllUniformLocation()
{
	for (auto & v : varmap)
	{
		v.second.location = GetUniformLocation(v.second.name);
	}
	unsigned int samplercounter = 0;
	for (auto & v : varmap)
	{
		switch (v.second.type)
		{
		case GL_TEXTURE_1D:
		case GL_TEXTURE_2D:
		case GL_TEXTURE_3D:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_DEPTH:
			glUniform1i(v.second.location, samplercounter);
			v.second.location = samplercounter;
			samplercounter++;
			break;
		default:
			break;
		}
	}
}
