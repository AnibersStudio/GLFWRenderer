#pragma once

#include <vector>

#include "GLCommon.h"
#include "Mesh.h"
#include "ShaderMaterial.h"
#include "Vertex.h"

class MeshManager
{
public:
	virtual const std::vector<ShaderMaterial>& GetShaderMatList() = 0;
	virtual const std::vector<MaterialedVertex>& GetOpaqueVertex() = 0;
	virtual const std::vector<MaterialedVertex>& GetFulltransVertex() = 0;
	virtual const std::vector<MaterialedVertex>& GetTransVertex() = 0;
	virtual const std::vector<unsigned int>& GenerateTransTask(glm::vec3 eye) = 0;
	///// <summary> Append a list of meshes of materials, vertex counts and vertices </summary>
	//virtual void AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> & vec, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const = 0;
	///// <summary> Append a list of meshes of materials, vertex counts and vertices in a near-to-far order </summary>
	//virtual void AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> & vec, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const = 0;
	///// <summary> Append a list of vertex positions </summary>
	//virtual void AppendPosition(unsigned char meshoption, std::vector<glm::vec3> & positionlist) const = 0;
};