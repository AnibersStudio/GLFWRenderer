#include "Renderer.h"
#include "CommonTools.h"

Renderer::Renderer(unsigned int w, unsigned int h) : width(w), height(h),
pointcon(ShaderController({ { "Shader/PointVertex.glsl", GL_VERTEX_SHADER }, { "Shader/PointFragment.glsl", GL_FRAGMENT_SHADER } })),
vao(Vao({ {2, GL_FLOAT}, {3, GL_FLOAT} }))
{
	glViewport(0, 0, w, h);
}

void Renderer::Draw(RenderObject& object)
{
	std::vector<Point> & point = object.rasterized;
	if (point.size())
	{
		vao.SetData(&point[0].position.x, sizeof(Point) * point.size());
	}
	vao.Bind();
	pointcon.Draw();
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, point.size());
}
