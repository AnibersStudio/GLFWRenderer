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
	void SetPosition(const glm::vec3 & pos) { position = pos; }
	void SetTexcoord(const glm::vec2 & texpos) { texcoord = texpos; }
	void SetNormal(const glm::vec3 & n) { normal = n; };
	void SetTangent(const glm::vec3 & t) { tangent = t; }
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent;
};

struct TexturedMaterial : public Material
{
	TexturedMaterial() {}
	TexturedMaterial(const glm::vec3 & a, const glm::vec3 & d, const glm::vec3 & s, const glm::vec3 & e, GLfloat sh, GLfloat tr) : Material(a, d, s, e, sh, tr) {}

	const Texture2D * diffusetex = nullptr;
	const Texture2D * speculartex = nullptr;
	const Texture2D * normaltex = nullptr;
	const Texture2D * emissivetex = nullptr;
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
	typedef std::vector<Vertex> MeshVert;
	typedef std::unordered_map<TexturedMaterial, std::vector<unsigned int>> MeshInd;
	IndexedModel() = default;
	IndexedModel(const std::string & objpath, bool isimportant = false, bool issurfacesmooth = false);
	void Transform(const glm::mat4 & transformmatrix);
	const MeshVert & GetMeshVert() const { return meshv; };
	const MeshInd & GetMeshInd() const { return meshi; };
	const std::string & GetPath() const { return path; }
protected:
	void ProcessNode(const aiNode * node, const aiScene * scene);
	void ProcessMesh(const aiMesh * mesh, const aiScene * scene);
	TexturedMaterial ProcessMaterial(const aiMaterial * material);
	std::string path;
private:
	MeshVert meshv;
	MeshInd meshi;
	bool iscolorsmooth;
	bool isnormalsmooth;
};

class ArrayModel
{
public:
	typedef std::vector<Vertex> MeshVert;
	typedef std::unordered_map<TexturedMaterial, MeshVert> Mesh;
	ArrayModel() = default;
	ArrayModel(const IndexedModel & im);
	void Transform(const glm::mat4 & transformmatrix);
	const Mesh & GetMesh() const;
	void Add(const TexturedMaterial & material, const MeshVert & verlist);
	ArrayModel & operator += (const ArrayModel & rhs);
protected:
	std::string path;
	Mesh mesh;

};

class PositionedArrayModel : public ArrayModel
{
public:
	PositionedArrayModel() = default;
	PositionedArrayModel(ArrayModel & model) : ArrayModel(model) {}
	PositionedArrayModel(ArrayModel & model, glm::vec3 p) : ArrayModel(model) { Position = p; }
	glm::vec3 Position;
};

class TextureLoader
{
private:
	static std::unordered_map <const std::tuple<std::string, bool, bool>, const Texture2D * > texmap;
public:
	static const Texture2D * Load2DTexture(std::string& path, bool issmooth, bool islinear = true, bool issRGB = false);
};

namespace std
{
	template <>
	struct hash <const std::tuple<std::string, bool, bool>>
	{
		std::size_t operator()(const std::tuple<std::string, bool, bool> obj) const
		{
			return std::hash<std::string>()(std::get<0>(obj)) ^ std::hash<bool>()(std::get<1>(obj)) ^ std::hash<bool>()(std::get<2>(obj));
		}
	};
}