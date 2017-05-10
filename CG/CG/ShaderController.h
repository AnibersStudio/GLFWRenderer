///Name: ShaderController
///Description: ShaderController takes a series of shader file and controlls how to use them and how to send values into glsl variables(deleted, not needed for this CG homework)
///Usage: Construct to use some shader file. Draw() to use this shader program
///Coder: Hao Pang
///Date: 2017.4.26
#pragma once
#include <string>
#include <vector>
#include "GLcommon.h"
#include "ShaderSubmiter.h"

class ShaderController : public ShaderSubmiter
{
public:
	/// <summary> Input a list of shader file to compile. Throws DrawErrorException if compile fails or get fails </summary>
	ShaderController(std::vector<std::pair<std::string, GLenum>> shaderlist);
	/// <summary> Prepare to draw, enabling the shader program. Must be called after all necessary Set()! </summary>
	void Draw();

	GLuint GetID() { return programid; }

protected:
	GLuint programid;

private:
	void ConstructShader(std::vector<std::pair<std::string, GLenum>> shaderlist);
	std::string ReadFile(std::string path);
};