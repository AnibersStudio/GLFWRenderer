///Name: RenderVao
///Description: A class who creates Vertex Array Objects and manages them
///Usage: Construct to create a VAO( and one or two VBOs), SetData*() to convey data from CPU to GPU, Bind() to bind vao for next drawcall
///Coder: Hao Pang
///Date: 2017.4.26
#pragma once

#include <vector>

#include "CommonTools.h"
#include "GLCommon.h"

struct VaoRecord
{
	/// <summary> The number of values specified by valuetype in vbo </summary>
	unsigned int valuecount;
	/// <summary> The value type of values specified by valuecount in vbo </summary>
	GLenum valuetype;
	/// <summary> The attribute is instanced or not. </summary>
	bool instanced = false;
};

class Vao
{
public:
	/// <summary> Construct a vao whose attribute list is provided. Throws DrawErrorException if valuetype not supported </summary>
	Vao(std::initializer_list<VaoRecord> attriblist, GLenum UsageHint = GL_STREAM_DRAW) : Vao(std::vector<VaoRecord>(attriblist), UsageHint) {};
	/// <summary> Construct a vao whose attribute list is provided. Throws DrawErrorException if valuetype not supported </summary>
	Vao(std::vector<VaoRecord> attriblist, GLenum UsageHint = GL_STREAM_DRAW);
	/// <summary> Set data of a vbo. Size in bytes. Apply for more vram if size not enough </summary>
	void SetData(const void * dataptr, size_t size);
	/// <summary> Set data of an instance vbo. Size in bytes. Apply for more vram if size not enough </summary>
	void SetInstanceData(const void * dataptr, size_t size);
	/// <summary> Bind vbo to be the source of draw </summary>
	void Bind() const { glBindVertexArray(vao); }

	GLuint GetVaoID() const { return vao; }
	GLuint GetVboID() const { return vbo; }
	size_t GetSize() const { return buffersize; }
private:
	size_t GetSizeofType(GLenum type);
	GLuint vao;
	GLuint vbo;
	GLuint instancevbo;
	GLenum usagehint;
	size_t buffersize = 0;
	size_t instancesize = 0;
};