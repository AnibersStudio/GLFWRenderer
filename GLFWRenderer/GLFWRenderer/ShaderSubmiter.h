#pragma once
#include "GLCommon.h"
#include "CommonTools.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <boost/any.hpp>

class ShaderSubmiter
{
public:
	/// <summary> Use the shader program. </summary>
	void Use() const
	{
		glUseProgram(shaderprogram);
	};
protected:
	/// <summary> 
	/// Add a shader file to compile. If compile fails, throws a DrawErrorException.
	/// <para>Must be called before LikeShader() ! </para>
	/// </summary>
	/// <param name="shadertype"> Vertex, fragment, geometry, etc. </param>
	/// <param name="shadertext"> The content of the shader file </param>
	void AddShader(GLenum shadertype, const char * shadertext);

	/// <summary> Link the shader objects compiled. If compile fails, throws a DrawErrorException  </summary>
	void LinkShader();

	/// <summary> Get uniform location of a shader variable. If get fails, throws a DrawErrorException </summary>
	/// <param name="uniformname"> Get an uniform of this name </param>
	GLuint GetUniformLocation(const std::string & uniformname) const;

	/// <summary> The shader program id in GPU </summary>
	GLuint shaderprogram;

private:
	std::vector<GLuint> shaderobjlist;
};

