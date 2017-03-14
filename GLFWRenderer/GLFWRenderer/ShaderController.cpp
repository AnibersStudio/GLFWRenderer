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
			ShaderErrorException exp("", logadd);
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
			ShaderErrorException exp("", logadd);
			delete[] logadd;
			throw exp;
		}
	}
}

GLuint ShaderController::GetUniformLocation(const std::string & uniformname) const
{
	GLuint uniformloc = glGetUniformLocation(shaderprogram, uniformname.c_str());
	if (uniformloc == 0xFFFFFFFF)
		throw ShaderErrorException{tostr(shaderprogram) + ":" + uniformname, "Error getting location of uniform. Maybe optimized out by compiler."};
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
		v.second.isset = false;
	}
}
//TODO
void LightShaderController::Set(std::string name, boost::any value)
{
	for (auto & v : varmap)
	{
		
	}
}
//TODO
void LightShaderController::Safe()
{
}
//TODO
void LightShaderController::Draw()
{
}

void LightShaderController::ConstructShader(std::vector<std::pair<std::string, GLenum>> shaderlist)
{
	for (auto shader : shaderlist)
	{
		AddShader(shader.second, ReadFile(shader.first).c_str());
	}
	LinkShader();
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
}
