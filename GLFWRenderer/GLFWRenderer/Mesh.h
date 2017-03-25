#pragma once
#pragma once

#include "GLCommon.h"
#include "Light.h"
#include "Texture2D.h"
#include "TexturedMaterial.h"
#include <string>
#include <unordered_map>
#include "Dependencies/include/assimp/Importer.hpp"
#include "Dependencies/include/assimp/scene.h"
#include "Dependencies/include/assimp/postprocess.h"
#include <tuple>

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent;
};

class IndexedModel
{
public:
	/// <summary> All of the vertices of the mesh </summary>
	typedef std::vector<Vertex> MeshVert;
	/// <summary> Map from material to indices to MeshVert </summary>
	typedef std::unordered_map<TexturedMaterial, std::vector<unsigned int>> MeshInd;
	/// <summary> Create a empty IndexedModel </summary>
	IndexedModel() = default;
	/// <summary> Load a mesh from file </summary>
	IndexedModel(const std::string & objpath);
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

class ArrayModel
{
public:
	/// <summary> All of the ordered vertices of the mesh </summary>
	typedef std::vector<Vertex> MeshVert;
	/// <summary> Mesh is a map from material to MeshVert </summary>
	typedef std::unordered_map<TexturedMaterial, MeshVert> Mesh;
	/// <summary> Create a default ArrayModel </summary>
	ArrayModel() = default;
	/// <summary> Create a Mesh from a IndexedModel </summary>
	ArrayModel(const IndexedModel & im);

	/// <summary> Apply a transform to mesh </summary>
	void Transform(const glm::mat4 & transformmatrix);
	/// <summary> Vertex maps of the mesh </summary>
	const Mesh & GetMesh() const;
	/// <summary> Add a mesh to current mesh </summary>
	void Add(const TexturedMaterial & material, const MeshVert & verlist);

	ArrayModel & operator += (const ArrayModel & rhs);

	glm::vec3 position;
protected:
	std::string path;
	Mesh mesh;
};