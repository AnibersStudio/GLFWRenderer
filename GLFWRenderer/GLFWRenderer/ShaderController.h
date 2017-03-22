#pragma once
#include <string>
#include <vector>
#include <boost/any.hpp>
#include "GLcommon.h"
#include "ShaderSubmiter.h"

/// <summary> A record of shader variable </summary>
struct ShaderVarRec
{
	/// <summary> Name of the variable. Recommend to be the same as in glsl. </summary>
	std::string name;
	/// <summary> Location of the variable in GPU.
	/// <para> For texture, This isn't the location of the sampler, but the binding point. </para>
	/// <para> For UBO, This isn't the location of the shader uniform, but the buffer object. </para>
	/// </summary>
	GLuint location;
	/// <summary> Type of the variable. Borrows GL enums. </summary>
	GLenum type;
	/// <summary> The default value of the variable. An empty value means no need for default value 
	/// <para> For texture, this should be the texture object id </para>
	/// <para> For UBO, this should be std::pair(const void * data, unsigned int count) </para>
	/// </summary>
	boost::any safevalue;
	/// <summary> The variable is set or not </summary>
	bool isset;
};

class ShaderController : public ShaderSubmiter
{
public:
	/// <summary> Input a list of shader file to compile. Throws DrawErrorException if compile fails or get fails </summary>
	ShaderController(std::initializer_list<std::pair<std::string, GLenum>> shaderlist, std::initializer_list<ShaderVarRec> variablelist);
	/// <summary> Input a list of shader file to compile. Throws DrawErrorException if compile fails or get fails </summary>
	ShaderController(std::vector<std::pair<std::string, GLenum>> shaderlist, std::vector<ShaderVarRec> variablelist);
	/// <summary> Clear state before a new round of uniform streaming. Must be called before Set()! </summary>
	void Clear();
	/// <summary> Set a uniform whose record was added in construction. Throws DrawErrorException if not added in construction.
	/// <para> Must be called between Clear() and Draw()! </para>
	/// </summary>
	void Set(std::string name, boost::any value);
	/// <summary> Set safe value for uniforms who hasn't been set.
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