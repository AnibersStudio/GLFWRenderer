#include "Rasterizer.h"
#include <iostream>
void RenderObject::NextVertex(Vertex vertex)
{
	existed.push_back(vertex);
	std::cout << "permanent.NextVertex({ vec2(" << vertex.position.x << "," << vertex.position.y << "), vec3(1.0) });" << std::endl;
}

void RenderObject::CloseVertex()
{
	switch (last)
	{
	case GL_LINE:
		if (existed.size() >= 2)
		{
			line.push_back(existed[existed.size() - 2]);
			line.push_back(existed[existed.size() - 1]);
		}
		break;
	case GL_POLYGON:
	case GL_POINTS:
		if (existed.size() >= 3)
		{
			for (int i = 2; i != existed.size(); i++)
			{
				triangle.push_back(existed[0]);
				triangle.push_back(existed[i - 1]);
				triangle.push_back(existed[i]);
			}
		}
		break;
	case GL_POINT:
		if (existed.size() >= 3)
		{
			for (int i = 0; i != 2; i++)
			{
				circle.push_back(existed[i]);
			}
			std::vector<Vertex> circlearcs;
			for (int i = 2; i != existed.size(); i++)
			{
				circlearcs.push_back(existed[i]);
			}
			arc.push_back(circlearcs);
		}
		break;
	}
	if (last == GL_POINTS)
	{
		rasterizer->Rasterize(*this, { {true, true}, {false, false}, {false, false}, {true, true} });
	}
	else
	{
		rasterizer->Rasterize(*this);
	}
}

void RenderObject::New()
{
	existed.clear();
}

void RenderObject::New(GLenum type)
{
	last = type;
	existed.clear();
}

void RenderObject::Clear()
{
	line.clear();
	triangle.clear();
	arc.clear();
	circle.clear();
	rasterized.clear();
	existed.clear();
}

void RenderObject::ClearVertex()
{
	line.clear();
	triangle.clear();
	circle.clear();
	arc.clear();
}

void RenderObject::Rasterize()
{
	rasterizer->Rasterize(*this);
}

RenderObject RenderObject::operator+(const RenderObject & rhs) const
{
	RenderObject renderobject = *this;
	for (auto v : rhs.line)
	{
		renderobject.line.push_back(v);
	}
	for (auto v : rhs.triangle)
	{
		renderobject.triangle.push_back(v);
	}
	for (auto v : rhs.rasterized)
	{
		renderobject.rasterized.push_back(v);
	}
	for (auto v : rhs.circle)
	{
		renderobject.circle.push_back(v);
	}
	for (auto v : rhs.arc)
	{
		renderobject.arc.push_back(v);
	}
	return renderobject;
}

void Rasterizer::Rasterize(RenderObject & object, std::vector<std::vector<bool>> shadow)
{
	std::vector<Vertex> ipoint;
	RasterizeLine(object.line, ipoint);
	RasterizeTriangle(object.triangle, ipoint, shadow);
	RasterizeCircle(object.circle, object.arc, ipoint);
	object.ClearVertex();
	std::vector<Point>& point = object.rasterized;
	for (auto v : ipoint)
	{
		point.push_back(Point{ (glm::vec2(v.position.x, height - v.position.y) + 0.5f) / glm::vec2(width, height) * 2.0f - 1.0f, v.color });
	}
}

void Rasterizer::RasterizeLine(std::vector<Vertex> line, std::vector<Vertex>& point)
{
	for (unsigned int i = 0; i < line.size(); i += 2)
	{
		Vertex v0 = line[i], v1 = line[i + 1];
		{
			unsigned int batchsize = point.size();
			if (v0.position.x == v1.position.x && v0.position.y == v1.position.y)
			{
				point.push_back(v0);
			}
			else
			{
				if (v0.position.x > v1.position.x)
				{
					Vertex tmp = v0;
					v0 = v1;
					v1 = tmp;
				}

				float slope = (v1.position.y - v0.position.y) / (float)(v1.position.x - v0.position.x);
				float m = slope;
				if (m < 0)//Up-down mirror
				{
					v0.position.y = height - v0.position.y;
					v1.position.y = height - v1.position.y;
					m = -m;
				}
				if (m > 1)//bisector mirror
				{
					v0.position = glm::ivec2(v0.position.y, v0.position.x);
					v1.position = glm::ivec2(v1.position.y, v1.position.x);
					m = 1 / m;
				}

				if (m >= 0 && m <= 1)
				{
					glm::vec3 t = (v1.color - v0.color) / (float)(v1.position.x - v0.position.x);

					int y = v0.position.y;
					int d = 2 * (v0.position.y - v1.position.y) * (v0.position.x + 1) + (v1.position.x - v0.position.x) * (2 * v0.position.y + 1) + 2 * v0.position.x * v1.position.y - 2 * v1.position.x * v0.position.y;
					for (int x = v0.position.x; x != v1.position.x; x++)
					{

						point.push_back(Vertex{ glm::vec2{ x, y },{ ((float)x - v0.position.x) * t + v0.color } });
						if (d < 0)
						{
							y++;
							d = d + 2 * (v1.position.x - v0.position.x) + 2 * (v0.position.y - v1.position.y);
						}
						else
						{
							d = d + 2 * (v0.position.y - v1.position.y);
						}
					}
				}

				for (auto & it = point.begin() + batchsize; it != point.end(); it++)
				{
					auto & p = *it;
					if (abs(slope) > 1)
					{
						p.position = glm::ivec2(p.position.y, p.position.x);
					}
					if (slope < 0)
					{
						p.position.y = height - p.position.y;
					}
				}
			}

		}
	}
}

void Rasterizer::RasterizeTriangle(std::vector<Vertex> triangle, std::vector<Vertex>& point, std::vector<std::vector<bool>> shadow)
{
	for (unsigned int i = 0; i + 2 < triangle.size(); i += 3)
	{
		Vertex v0 = triangle[i], v1 = triangle[i + 1], v2 = triangle[i + 2];
		unsigned int xmin = glm::floor(glm::min(glm::min(v0.position.x, v1.position.x), v2.position.x));
		unsigned int xmax = glm::ceil(glm::max(glm::max(v0.position.x, v1.position.x), v2.position.x));
		unsigned int ymin = glm::floor(glm::min(glm::min(v0.position.y, v1.position.y), v2.position.y));
		unsigned int ymax = glm::ceil(glm::max(glm::max(v0.position.y, v1.position.y), v2.position.y));
		auto f = [v0, v1, v2](int first, int second, float x, float y)
		{ glm::vec2 vertex[3]{ v0.position, v1.position, v2.position }; return (vertex[first].y - vertex[second].y) * x + (vertex[second].x - vertex[first].x) * y
			+ vertex[first].x * vertex[second].y - vertex[second].x * vertex[first].y; };
		float falpha = f(1, 2, v0.position.x, v0.position.y), fbeta = f(2, 0, v1.position.x, v1.position.y), fgama = f(0, 1, v2.position.x, v2.position.y);
		for (unsigned int y = ymin; y < ymax; y++)
		{
			for (unsigned int x = xmin; x < xmax; x++)
			{
				float alpha = f(1, 2, x, y) / falpha, beta = f(2, 0, x, y) / fbeta, gama = f(0, 1, x, y) / fgama;
				if (alpha >= 0 && beta >= 0 && gama >= 0)
				{
					if ((alpha > 0 || falpha * f(1, 2, -1, -1) > 0) && (beta > 0 || fbeta * f(2, 0, -1, -1) > 0) && (gama > 0 || fgama * f(0, 1, -1, -1) > 0))
					{
						glm::vec3 color = alpha * v0.color + beta * v1.color + gama * v2.color;
						if (shadow.size())
						{
							int w = shadow.size();
							int h = shadow[0].size();
							if (shadow[x%w][y%h])
							{
								point.push_back(Vertex{ glm::vec2{ x, y }, color });
							}
						}
						else
						{
							point.push_back(Vertex{ glm::vec2{ x, y }, color });
						}
					}
				}
			}
		}
	}
}

void Rasterizer::RasterizeCircle(std::vector<Vertex> circle, std::vector<std::vector<Vertex>> arc, std::vector<Vertex>& point)
{
	int i;
	std::vector<Vertex> temppoint;
	for (i = 0; i != circle.size(); i += 2)
	{
		int index = i / 2;
		Vertex v0 = circle[i], v1 = circle[i + 1];
		glm::vec2 center = (v0.position + v1.position) / 2.0f;
		float a = glm::max(abs(v0.position.y - center.y), abs(v0.position.x - center.x));
		float b = glm::min(abs(v0.position.y - center.y), abs(v0.position.x - center.x));
		int x = 0, y = b;
		double fx = a * a / sqrt((double)a * a + b * b);
		double d = 4 * b * b + a * a * (-4 * b + 1);

		glm::vec2 offset[4] = { glm::vec2{ 1.0f, 1.0f }, glm::vec2{ 1.0f, -1.0f }, glm::vec2{ -1.0f, -1.0f }, glm::vec2{ -1.0f, 1.0f } };
		temppoint.push_back(Vertex{ center + glm::vec2(x, y), v0.color });
		temppoint.push_back(Vertex{ center + glm::vec2(x, -y), v0.color });
		while (x <= fx)
		{
			if (d < 0)
				d += 4 * b * b * (2 * x + 3);
			else
			{
				--y;
				d += 4 * b * b * (2 * x + 3) + 4 * a * a * (-2 * y + 2);
			}
			++x;

			for (int i = 0; i != 4; i++)
			{
				temppoint.push_back(Vertex{ center + glm::vec2(x, y) * offset[i], v0.color });
			}
		}
		x = glm::round(fx);
		d = b * b *(2 * x + 1) * (2 * x + 1) + 4 * a * a * (y - 1) * (y - 1) - 4 * a * a * b * b;
		while (y > 0)
		{
			if (d < 0)
			{
				++x;
				d += 4 * b * b * (2 * x + 2) + 4 * a * a *(-2 * y + 3);
			}
			else
				d += 4 * a * a * (-2 * y + 3);
			--y;
			for (int i = 0; i != 4; i++)
			{
				temppoint.push_back(Vertex{ center + glm::vec2(x, y) * offset[i], v0.color });
			}
		}
		if (abs(v0.position.x - v1.position.x) < abs(v0.position.y - v1.position.y))
		{
			for (auto & it = temppoint.begin(); it != temppoint.end(); it++)
			{
				auto & p = *it;
				p.position -= center;
				p.position = glm::vec2(p.position.y, p.position.x);
				p.position += center;
			}
		}
		for (auto p : temppoint)
		{
			bool draw = arc[index].size() < 2;
			glm::vec2 dir = glm::normalize(p.position - center);
			for (unsigned int testarc = 1; testarc < arc[index].size(); testarc ++)
			{
				glm::vec2 arc0 = glm::normalize(arc[index][testarc - 1].position - center);
				glm::vec2 arc1 = glm::normalize(arc[index][testarc].position - center);
				float dot0 = glm::dot(arc0, dir);
				float dot1 = glm::dot(arc1, dir);
				float refdot = glm::dot(arc0, arc1);
				if (dot0 >= refdot && dot1 >= refdot)
				{
					draw = true;
				}
			}
			if (draw)
			{
				point.push_back(p);
			}
		}
	}
}
