#include "BufferObjectSubmiter.h"
#include "CommonTools.h"
GLuint BufferObjectSubmiter::Generate(size_t size)
{
	GLuint bufferID;
	glGenBuffers(1, &bufferID);
	buffersize[bufferID] = size;
	if (size)
	{
		glNamedBufferDataEXT(bufferID, size, NULL, GL_STREAM_DRAW);
	}
	return bufferID;
}

void BufferObjectSubmiter::SetData(GLuint bufferID, const void * data, size_t size, GLenum hint)
{
	if (buffersize.find(bufferID) == buffersize.end())
	{
		throw DrawErrorException("BufferObjectSubmiter:BufferID " + tostr(bufferID), "This ID isn't generated by This submiter.");
	}

	if (buffersize[bufferID] < size)
	{
		glNamedBufferDataEXT(bufferID, size, data, hint);
		buffersize[bufferID] = size;
	}
	else
	{
		glNamedBufferSubDataEXT(bufferID, 0, size, data);
	}
}

void BufferObjectSubmiter::Reserve(GLuint bufferID, size_t size, GLenum hint)
{
	if (buffersize.find(bufferID) == buffersize.end())
	{
		throw DrawErrorException("BufferObjectSubmiter:BufferID " + tostr(bufferID), "This ID isn't generated by This submiter.");
	}
	if (buffersize[bufferID] < size)
	{
		glNamedBufferDataEXT(bufferID, size, nullptr, hint);
		buffersize[bufferID] = size;
	}
}


