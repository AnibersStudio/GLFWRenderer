#include "RenderVao.h"
#include <unordered_map>

Vao::Vao(std::vector<VaoRecord> attriblist, GLenum UsageHint)
{
	usagehint = UsageHint;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &instancevbo);
	size_t sizepervertex = 0, sizeperinstance = 0;
	for (auto & attrib : attriblist)
	{
		if (!attrib.instanced)
		{
			sizepervertex += GetSizeofType(attrib.valuetype) * attrib.valuecount;
		}
		else
		{
			sizeperinstance += GetSizeofType(attrib.valuetype) * attrib.valuecount;
		}
	}
	size_t vertexstride = 0, instancestride = 0;
	for (int i = 0; i != attriblist.size(); i++)
	{
		if (!attriblist[i].instanced)
		{
			glVertexArrayVertexAttribOffsetEXT(vao, vbo, i, attriblist[i].valuecount, attriblist[i].valuetype, GL_FALSE, sizepervertex, vertexstride);
			glEnableVertexArrayAttribEXT(vao, i);
			vertexstride += GetSizeofType(attriblist[i].valuetype) * attriblist[i].valuecount;
		}
		else
		{
			glVertexArrayVertexAttribOffsetEXT(vao, instancevbo, i, attriblist[i].valuecount, attriblist[i].valuetype, GL_FALSE, sizeperinstance, instancestride);
			glEnableVertexArrayAttribEXT(vao, i);
			glVertexArrayVertexAttribDivisorEXT(vao, i, 1);
			instancestride += GetSizeofType(attriblist[i].valuetype) * attriblist[i].valuecount;
		}

	}
}

void Vao::SetData(const void * dataptr, size_t size)
{
	if (size > buffersize)
	{
		glNamedBufferDataEXT(vbo, size, dataptr, usagehint);
		buffersize = size;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, dataptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Vao::SetInstanceData(const void * dataptr, size_t size)
{
	if (size > instancesize)
	{
		glNamedBufferDataEXT(instancevbo, size, dataptr, usagehint);
		instancesize = size;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, dataptr);
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
