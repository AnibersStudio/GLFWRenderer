#pragma once
#include "GLCommon.h"
#include "CommonTools.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <boost/any.hpp>

/// <summary> A exception for shader compiling and linking </summary>
struct ShaderErrorException : public std::exception
{
	ShaderErrorException(std::string p,std::string e) : path(p), glerror(e) {}
	/// <summary> Path or id of the shader file </summary>
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

class ShaderController
{
public:
	/// <summary> Use the shader program. </summary>
	void Use() const
	{
		glUseProgram(shaderprogram);
	};
protected:
	/// <summary> 
	/// Add a shader file to compile. If compile fails, throws an ShaderErrorException.
	/// <para>Must be called before LikeShader() ! </para>
	/// </summary>
	/// <param name="shadertype"> Vertex, fragment, geometry, etc. </param>
	/// <param name="shadertext"> The content of the shader file </param>
	void AddShader(GLenum shadertype, const char * shadertext);

	/// <summary> Link the shader objects compiled. If compile fails, throws an ShaderErrorException  </summary>
	void LinkShader();

	/// <summary> Get uniform location of a shader variable. If get fails, throws an ShaderErrorException </summary>
	/// <param name="uniformname"> Get an uniform of this name </param>
	GLuint GetUniformLocation(const std::string & uniformname) const;

	/// <summary> The shader program id in GPU </summary>
	GLuint shaderprogram;

private:
	std::vector<GLuint> shaderobjlist;
};

/// <summary> A record of shader variable </summary>
struct ShaderVarRec
{
	/// <summary> Name of the variable. Recommend to be the same as in glsl. </summary>
	std::string name;
	/// <summary> Location of the variable in GPU.
	/// <para> For texture, This isn't the location of the sampler, but the binding point. </para>
	/// </summary>
	GLuint location;
	/// <summary> Type of the variable. Borrows GL enums. </summary>
	GLenum type;
	/// <summary> The default value of the variable. An empty value means no need for default value </summary>
	boost::any safevalue;
	/// <summary> The variable is set or not </summary>
	bool isset;
};

class LightShaderController : public ShaderController
{
public:
	/// <summary> Input a list of shader file to compile. Throws ShaderErrorException if compile fails or get fails </summary>
	LightShaderController(std::initializer_list<std::pair<std::string, GLenum>> shaderlist, std::initializer_list<ShaderVarRec> variablelist);
	/// <summary> Input a list of shader file to compile. Throws ShaderErrorException if compile fails or get fails </summary>
	LightShaderController(std::vector<std::pair<std::string, GLenum>> shaderlist, std::vector<ShaderVarRec> variablelist);
	/// <summary> Clear state before a new round of uniform streaming. Must be called before Set()! </summary>
	void Clear();
	/// <summary> Set a uniform whose record was added in construction. Throws ShaderErrorException if not added in construction.
	/// <para> Must be called between Set() and Draw()! </para>
	/// </summary>
	void Set(std::string name, boost::any value);
	/// <summary> Set safe value for uniforms who hasn't been set. Throws ShaderErrorException if not added in construction.
	/// <para> Recommend to call after uniform streaming </para>
	///</summary>
	void Safe();
	/// <summary> Set a list of uniform. The list must contain all of values to set because this function override itself </summary>
	void SetInOneShot(std::vector<std::pair<std::string, boost::any>> list);
	/// <summary> Prepare to draw, enabling the shader program. Must be called after all necessary Set()! </summary>
	void Draw();

protected:
	GLuint programid;

private:
	void ConstructShader(std::vector<std::pair<std::string, GLenum>> shaderlist);
	void ConstructVarMap(std::vector<ShaderVarRec> variablelist);
	std::string ReadFile(std::string path);
	void GetAllUniformLocation();

	std::unordered_map<std::string, ShaderVarRec> varmap;
};