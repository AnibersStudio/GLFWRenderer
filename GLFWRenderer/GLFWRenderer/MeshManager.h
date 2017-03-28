#pragma once

#include <vector>

#include "GLCommon.h"
#include "Mesh.h"

class MeshManager
{
public:
	/// <summary> Used to specify what kind of mesh to get. Use bitwise and to get multiple types </summary>
	static const unsigned char Opace = 1;
	/// <summary> Used to specify what kind of mesh to get. Use bitwise and to get multiple types </summary>
	static const unsigned char FullTrans = 2;
	/// <summary> Used to specify what kind of mesh to get. Use bitwise and to get multiple types </summary>
	static const unsigned char SemiTrans = 4;
	/// <summary> Get a list of meshes of materials, vertex counts and vertices </summary>
	virtual std::tuple<std::vector<TexturedMaterial>, std::vector<unsigned int>, std::vector<Vertex>> GetMeshList(unsigned char meshoption) const = 0;
	/// <summary> Get a list of meshes of materials, vertex counts and vertices in a near-to-far order </summary>
	virtual std::tuple<std::vector<TexturedMaterial>, std::vector<unsigned int>, std::vector<Vertex>> GetOrderedMeshList(unsigned char meshoption, glm::vec3 eye) const = 0;
	/// <summary> Get a list of vertex positions </summary>
	virtual std::vector<glm::vec3> GetPositionList(unsigned char meshoption) const = 0;

	/// <summary> Append a list of meshes of materials, vertex counts and vertices </summary>
	virtual void AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> vec, std::vector<unsigned int> count, std::vector<Vertex> vertices) const = 0;
	/// <summary> Append a list of meshes of materials, vertex counts and vertices in a near-to-far order </summary>
	virtual void AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> vec, std::vector<unsigned int> count, std::vector<Vertex> vertices) const = 0;
	/// <summary> Append a list of vertex positions </summary>
	virtual void AppendPosition(unsigned char meshoption, std::vector<glm::vec3> positionlist) const = 0;
};