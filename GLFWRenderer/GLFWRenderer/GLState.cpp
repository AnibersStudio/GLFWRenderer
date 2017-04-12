#include "GLState.h"

void GLState::ColdSet()
{
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
	
	glCullFace(cullface);
	glFrontFace(frontface);
}

void GLState::HotSet(GLState & oldstate)
{
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
