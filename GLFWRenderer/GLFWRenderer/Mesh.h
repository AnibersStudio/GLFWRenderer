#pragma once
#pragma once

#include "GLCommon.h"
#include "Texture2D.h"
#include "TexturedMaterial.h"
#include <string>
#include <unordered_map>
#include "Dependencies/include/assimp/Importer.hpp"
#include "Dependencies/include/assimp/scene.h"
#include "Dependencies/include/assimp/postprocess.h"
#include <tuple>
#include "Vertex.h"

class IndexedMesh
{
public:
	/// <summary> All of the vertices of the mesh </summary>
	using MeshVert = std::vector<Vertex> ;
	/// <summary> Map from material to indices to MeshVert </summary>
	using MeshInd = std::unordered_map<TexturedMaterial, std::vector<unsigned int>> ;
	/// <summary> Create a empty IndexedModel </summary>
	IndexedMesh() = default;
	/// <summary> Load a mesh from file </summary>
	IndexedMesh(const std::string & objpath);
	/// <summary> Apply a transform to the mesh </summary>

	void Transform(const glm::mat4 & transformmatrix);
	/// <summary> Mesh Vertices of the mesh </summary>
	const MeshVert & GetMeshVert() const { return meshv; };
	/// <summary> Mesh Indices map of the mesh </summary>
	const MeshInd & GetMeshInd() const { return meshi; };
	/// <summary> Path of the mesh file </summary>
	const std::string & GetPath() const { return path; }
private:
	void ProcessNode(const aiNode * node, const aiScene * scene);
	void ProcessMesh(const aiMesh * mesh, const aiScene * scene);
	TexturedMaterial ProcessMaterial(const aiMaterial * material);
	std::string path;
	MeshVert meshv;
	MeshInd meshi;
};

class ArrayMesh
{
public:
	/// <summary> All of the ordered vertices of the mesh </summary>
	using MeshVert = std::vector<Vertex> ;
	/// <summary> Mesh is a map from material to MeshVert </summary>
	using Mesh = std::unordered_map<TexturedMaterial, MeshVert> ;
	/// <summary> Create a default ArrayModel </summary>
	ArrayMesh() = default;
	/// <summary> Create a Mesh from a IndexedModel </summary>
	ArrayMesh(const IndexedMesh & im);

	/// <summary> Apply a transform to mesh </summary>
	void Transform(const glm::mat4 & transformmatrix);
	/// <summary> Vertex maps of the mesh </summary>
	const Mesh & GetMesh() const;
	/// <summary> Add a mesh to current mesh </summary>
	void Add(const TexturedMaterial & material, const MeshVert & verlist);
	/// <summary> Get count of vertex of the mesh
	unsigned int VertexCount();


	ArrayMesh & operator += (const ArrayMesh & rhs);

	glm::vec3 position;
protected:
	std::string path;
	Mesh mesh;
};