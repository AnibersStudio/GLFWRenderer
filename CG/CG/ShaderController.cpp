#include <fstream>
#include "ShaderController.h"
ShaderController::ShaderController(std::vector<std::pair<std::string, GLenum>> shaderlist)
{
	ConstructShader(shaderlist);
}

void ShaderController::Draw()
{
	Use();
}

void ShaderController::ConstructShader(std::vector<std::pair<std::string, GLenum>> shaderlist)
{
	for (auto shader : shaderlist)
	{
		try {
			std::string shadersource = ReadFile(shader.first);
			if (!shadersource.size()) throw DrawErrorException("", "Cannot read shader file. Wrong path?");
			AddShader(shader.second, shadersource.c_str());
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