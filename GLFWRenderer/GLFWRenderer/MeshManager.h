#pragma once

#include <vector>

#include "GLCommon.h"
#include "Mesh.h"

class MeshManager
{
public:
	static const unsigned char Opace = 1;
	static const unsigned char FullTrans = 2;
	static const unsigned char SemiTrans = 4;
	virtual std::tuple<std::vector<TexturedMaterial>, std::vector<unsigned int>, std::vector<Vertex>> GetMeshList(unsigned char meshoption) const = 0;
	virtual std::tuple<std::vector<TexturedMaterial>, std::vector<unsigned int>, std::vector<Vertex>> GetOrderedMeshList(unsigned char meshoption, glm::vec3 eye) const = 0;
	virtual std::vector<glm::vec3> GetPositionList(unsigned char meshoption) const = 0;

	virtual void AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> vec, std::vector<unsigned int> count, std::vector<Vertex> vertices) const = 0;
	virtual void AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> vec, std::vector<unsigned int> count, std::vector<Vertex> vertices) const = 0;
	virtual void AppendPosition(unsigned char meshoption, std::vector<glm::vec3> positionlist) const = 0;
};