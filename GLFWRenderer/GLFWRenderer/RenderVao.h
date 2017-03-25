#pragma once

#include <vector>

#include "CommonTools.h"
#include "GLCommon.h"

struct VaoRecord
{
	/// <summary> The number of values specified by valuetype in vbo <summary>
	unsigned int valuecount;
	/// <summary> The value type of values specified by valuecount in vbo <summary>
	GLenum valuetype;
};

class Vao
{
public:
	/// <summary> Construct a vao whose attribute list is provided. Throws DrawErrorException if valuetype not supported <summary>
	Vao(std::initializer_list<VaoRecord> attriblist, GLenum UsageHint = GL_STREAM_DRAW) : Vao(std::vector<VaoRecord>(attriblist), UsageHint) {};
	/// <summary> Construct a vao whose attribute list is provided. Throws DrawErrorException if valuetype not supported <summary>
	Vao(std::vector<VaoRecord> attriblist, GLenum UsageHint = GL_STREAM_DRAW);
	/// <summary> Set data of a vbo. Apply for more vram if size not enough <summary>
	void SetData(const void * dataptr, size_t size);
	/// <summary> Bind vbo to be the source of draw <summary>
	void Bind() { glBindVertexArray(vao); }

	GLuint GetVaoID() { return vao; }
	GLuint GetVboID() { return vbo; }
	size_t GetSize() { return buffersize; }
private:
	size_t GetSizeofType(GLenum type);
	GLuint vao;
	GLuint vbo;
	GLenum usagehint;
	size_t buffersize = 0;
};