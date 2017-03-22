#pragma once
#include "Light.h"
#include "Texture2D.h"
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