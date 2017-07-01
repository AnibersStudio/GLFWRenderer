#pragma once

#include "Texture2D.h"

struct Material
{
	Material() = default;
	Material(const glm::vec3 & a, const glm::vec3 & d, const glm::vec3 & s, const glm::vec3 & e, GLfloat sh, GLfloat tr)
		: ambientcolor(a), diffusecolor(d), specularcolor(s), emissivecolor(e), shininess(sh), transparency(tr) {}
	glm::vec3 ambientcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	float not_used1[1];
	glm::vec3 diffusecolor = glm::vec3(0.0f, 0.0f, 0.0f);
	float not_used2[1];
	glm::vec3 specularcolor = glm::vec3(0.0f, 0.0f, 0.0f);
	float not_used3[1];
	glm::vec3 emissivecolor = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat shininess = 1000.0f;
	GLfloat transparency = 1.0f;
	float not_used4[3];
	bool operator==(const Material & rhs) const {
		return ambientcolor == rhs.ambientcolor && diffusecolor == rhs.diffusecolor &&
			specularcolor == rhs.specularcolor && emissivecolor == rhs.emissivecolor &&
			shininess == rhs.shininess && transparency == rhs.transparency;
	}
};

namespace std
{
	template <>
	struct hash <Material>
	{
		std::size_t operator()(const Material & obj) const
		{
			return std::hash<float>()(obj.ambientcolor[0]) ^ std::hash<float>()(obj.ambientcolor[1]) ^ std::hash<float>()(obj.ambientcolor[2])
				^ std::hash<float>()(obj.diffusecolor[0]) ^ std::hash<float>()(obj.diffusecolor[1]) ^ std::hash<float>()(obj.diffusecolor[2])
				^ std::hash<float>()(obj.specularcolor[0]) ^ std::hash<float>()(obj.specularcolor[1]) ^ std::hash<float>()(obj.specularcolor[2])
				^ std::hash<float>()(obj.emissivecolor[0]) ^ std::hash<float>()(obj.emissivecolor[1]) ^ std::hash<float>()(obj.emissivecolor[2])
				^ std::hash<float>()(obj.shininess) ^ std::hash<float>()(obj.transparency);
		}
	};
}

struct TexturedMaterial : public Material
{
	/// <summary> Default constructor with default material parameters and none texture </summary>
	TexturedMaterial() {}
	/// <summary> Textured material with these parameters. Please assign textures directly </summary>
	TexturedMaterial(const glm::vec3 & a, const glm::vec3 & d, const glm::vec3 & s, const glm::vec3 & e, GLfloat sh, GLfloat tr) : Material(a, d, s, e, sh, tr) {}

	/// <summary> Albedo texture of the material. nullptr means none </summary>
	Texture2D * diffusetex = nullptr;
	/// <summary> Mentalness texture of the material. nullptr means none 
	/// <para> Only the red chanel of the texture is used. </para>
	/// </summary>
	Texture2D * speculartex = nullptr;
	/// <summary> Normal texture of the material. nullptr means none </summary>
	Texture2D * normaltex = nullptr;
	/// <summary> Emissive texture of the material. nullptr means none </summary>
	Texture2D * emissivetex = nullptr;
	/// <summary> Transparency texture of the material. nullptr means none 
	/// <para> Only the alpha chanel is used. </para>
	/// </summary>
	Texture2D * transtex = nullptr;

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