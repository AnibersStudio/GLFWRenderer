#include "RenderVao.h"
#include <unordered_map>

Vao::Vao(std::vector<VaoRecord> attriblist, GLenum UsageHint)
{
	usagehint = UsageHint;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	size_t sizepervertex = 0;
	for (auto & attrib : attriblist)
	{
		sizepervertex += GetSizeofType(attrib.valuetype) * attrib.valuecount;
	}
	size_t stride = 0;
	for (int i = 0; i != attriblist.size(); i++)
	{
		glVertexArrayVertexAttribOffsetEXT(vao, vbo, i, attriblist[i].valuecount, attriblist[i].valuetype, GL_FALSE, sizepervertex, stride);
		glEnableVertexArrayAttribEXT(vao, i);
		
		if (attriblist[i].instanced)
		{
			glVertexArrayVertexAttribDivisorEXT(vao, i, 1);
		}

		stride += GetSizeofType(attriblist[i].valuetype) * attriblist[i].valuecount;
	}
}

void Vao::SetData(const void * dataptr, size_t size) const
{
	if (size > buffersize)
	{
		glNamedBufferDataEXT(vbo, size, dataptr, usagehint);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(vbo, 0, size, dataptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

size_t Vao::GetSizeofType(GLenum type)
{
	static std::unordered_map<GLenum, size_t> typesizemap{ {GL_FLOAT, sizeof(GLfloat)},{GL_INT, sizeof(GLint)},{GL_UNSIGNED_INT, sizeof(GLuint)},
	{GL_BYTE, sizeof(GLbyte)}, {GL_UNSIGNED_BYTE, sizeof(GLubyte)}, {GL_DOUBLE, sizeof(GLdouble)} };
	size_t size = 0;
	try {
		size = typesizemap.at(type);
	}
	catch (std::out_of_range&)
	{
		throw DrawErrorException("Vao" + tostr(vao), "Type " + tostr(type) + " isn't supported in vao.");
	}
	return size;
}
