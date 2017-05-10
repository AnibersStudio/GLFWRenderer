#pragma once
#include <vector>
#include "GLCommon.h"

struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;
};

struct Point
{
	glm::vec2 position;
	glm::vec3 color;
};

class Rasterizer;

struct RenderObject
{
	std::vector<Vertex> line;
	std::vector<Vertex> triangle;
	std::vector<Vertex> circle;
	std::vector<std::vector<Vertex>> arc;
	std::vector<Point> rasterized;
	void NextVertex(Vertex vertex);
	void CloseVertex();
	void New();
	void New(GLenum type);
	void Clear();
	void ClearVertex();
	void Init(Rasterizer * rasptr) { rasterizer = rasptr; };
	void Rasterize();

	RenderObject operator + (const RenderObject & rhs) const;

	GLenum last;
private:
	Rasterizer * rasterizer;

	std::vector<Vertex> existed;
};

class Rasterizer
{
public:
	Rasterizer(unsigned int w, unsigned int h) : width(w), height(h) {}
	void Rasterize(RenderObject& object, std::vector<std::vector<bool>> shadow = {});
	void RasterizeLine(std::vector<Vertex> line, std::vector<Vertex> & point);
	void RasterizeTriangle(std::vector<Vertex> triangle, std::vector<Vertex> & point, std::vector<std::vector<bool>> shadow = {});
	void RasterizeCircle(std::vector<Vertex> circle, std::vector<std::vector<Vertex>> arc, std::vector<Vertex> & point);
private:
	unsigned int width, height;
};