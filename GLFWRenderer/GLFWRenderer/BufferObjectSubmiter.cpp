#include "BufferObjectSubmiter.h"
#include "CommonTools.h"
GLuint BufferObjectSubmiter::Generate()
{
	GLuint bufferID;
	glGenBuffers(1, &bufferID);
	buffersize[bufferID] = 0;
	return bufferID;
}

void BufferObjectSubmiter::SetData(GLuint bufferID, const void * data, size_t size, GLenum hint = GL_STREAM_DRAW)
{
#ifdef _DEBUG
	try {
		buffersize.at(bufferID);
	}
	catch (std::out_of_range&)
	{
		throw DrawErrorException("BufferObjectSubmiter:BufferID" + tostr(bufferID), "This ID isn't generated by This submiter.");
	}
#endif


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

