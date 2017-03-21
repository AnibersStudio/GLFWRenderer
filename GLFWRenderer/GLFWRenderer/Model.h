#pragma once

#include "GLCommon.h"
#include "Light.h"
#include "Texture.h"
#include <string>
#include <unordered_map>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <tuple>

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent;
};

struct TexturedMaterial : public Material
{
	/// <summary> Default constructor with default material parameters and none texture </summary>
	TexturedMaterial() {}
	/// <summary> Textured material with these parameters. Please assign textures directly </summary>
	TexturedMaterial(const glm::vec3 & a, const glm::vec3 & d, const glm::vec3 & s, const glm::vec3 & e, GLfloat sh, GLfloat tr) : Material(a, d, s, e, sh, tr) {}

	/// <summary> Albedo texture of the material. nullptr means none </summary>
	const Texture2D * diffusetex = nullptr;
	/// <summary> Mentalness texture of the material. nullptr means none 
	/// <para> Only the red chanel of the texture is used. </para>
	/// </summary>
	const Texture2D * speculartex = nullptr;
	/// <summary> Normal texture of the material. nullptr means none </summary>
	const Texture2D * normaltex = nullptr;
	/// <summary> Emissive texture of the material. nullptr means none </summary>
	const Texture2D * emissivetex = nullptr;
	/// <summary> Transparency texture of the material. nullptr means none 
	/// <para> Only the alpha chanel is used. </para>
	/// </summary>
	const Texture2D * transtex = nullptr;

	bool operator==(const TexturedMaterial & rhs) const
	{
		return diffusetex == rhs.diffusetex && speculartex == rhs.speculartex && normaltex == rhs.normaltex && emissivetex == rhs.emissivetex && transtex == rhs.transtex && Material::operator==(rhs);
	}
};


namespace std
{
	template <>
	struct hash <TexturedMaterial>
	{
		std::size_t operator()(const TexturedMaterial & obj) const
		{
			size_t d, s, e, n, tr;
			d = obj.diffusetex == nullptr ? std::hash<int>()(0) : std::hash<Texture2D>()(*obj.diffusetex);
			s = obj.speculartex == nullptr ? std::hash<int>()(0) : std::hash<Texture2D>()(*obj.speculartex);
			e = obj.emissivetex == nullptr ? std::hash<int>()(0) : std::hash<Texture2D>()(*obj.emissivetex);
			n = obj.normaltex == nullptr ? std::hash<int>()(0) : std::hash<Texture2D>()(*obj.normaltex);
			tr = obj.transtex == nullptr ? std::hash<int>()(0) : std::hash<Texture2D>()(*obj.transtex);
			return std::hash<Material>()(obj) ^ d ^ s ^ e ^ n;
		}
	};
}

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

class MeshLoader
{
public:
	static MeshLoader & GetInstance()
	{
		static MeshLoader meshloader;
		return meshloader;
	}
	/// <summary> Register mesh to open </summary>
	void Register(std::string name, std::string path)
	{
		MeshMap[name] = ArrayModel(path);
	}
	ArrayModel& Get(std::string name)
	{
		return MeshMap[name];
	}
	ArrayModel Get(std::string name, glm::mat4 transform)
	{
		auto model = MeshMap[name];
		model.Transform(transform);
		return model;
	}
private:
	MeshLoader() = default;
	std::unordered_map<std::string, ArrayModel> MeshMap;
};