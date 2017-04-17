#include "GLState.h"

void GLState::ColdSet()
{
	glViewport(0, 0, w, h);
	if (depthtest)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	glDepthFunc(depthfunc);
	glDepthMask(depthmask);
	if (blend)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	glBlendFunc(blendfunc.first, blendfunc.second);
	
	if (facetest)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	glCullFace(cullface);
	glFrontFace(frontface);
}

void GLState::HotSet(GLState & oldstate)
{
	if (w != oldstate.w || h != oldstate.h)
	{
		glViewport(0, 0, w, h);
	}
	if (depthtest != oldstate.depthtest)
	{
		if (depthtest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
	if (depthfunc != oldstate.depthfunc)
	{
		glDepthFunc(depthfunc);
	}
	if (depthmask != oldstate.depthmask)
	{
		glDepthMask(depthmask);
	}
	if (blend != oldstate.blend)
	{
		if (blend)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	if (blendfunc != oldstate.blendfunc)
	{
		glBlendFunc(blendfunc.first, blendfunc.second);
	}
	
	if (facetest != oldstate.facetest)
	{
		if (facetest)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}
	if (cullface != oldstate.cullface)
	{
		glCullFace(cullface);
	}
	
	if (frontface != oldstate.frontface)
	{
		glFrontFace(frontface);
	}
	oldstate = *this;
}
