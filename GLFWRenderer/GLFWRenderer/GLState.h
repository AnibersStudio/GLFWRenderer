#pragma once

#include <iostream>

#include "GLCommon.h"


struct GLState
{
	/// <summary> Enable depth test or not. Default is true(test) </summary>
	bool depthtest = true;
	/// <summary> The depth function of depth test. Default is GL_LESS(front drawn) </summary>
	GLenum depthfunc = GL_LESS;
	/// <summary> Blend or not. Default is false(opace) </summary>
	bool blend = false;
	/// <summary> The blend function of blending. Default is GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA </summary>
	std::pair<GLenum, GLenum> blendfunc{GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA};
	/// <summary> Cull which face. GL_NONE, GL_FRONT or GL_BACK(default) </summary>
	GLenum cullface = GL_BACK;
	/// <summary> Specify the vertex order of front face. Could be GL_CW or GL_CCW(default) </summary>
	GLenum frontface = GL_CCW;

	void ColdSet();
	void HotSet(GLState & oldstate);
};