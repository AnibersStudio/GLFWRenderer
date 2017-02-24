#pragma once
#include "GLCommon.h"
//Lights Defs
using namespace glm;
struct BaseLight
{
	BaseLight(float i, vec3 c, float d, float s) : intensity(i), color(c), diffuse(d), specular(s) {}
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	float not_used_for_alignment[2] = { 0.0f, 0.0f };
};

struct DirectionalLight : public BaseLight
{
	DirectionalLight() : BaseLight(0, glm::vec3(1.0), 1.0, 0.2) {}
	DirectionalLight(float i, vec3 c, float d, float s, vec3 dir) : BaseLight(i, c, d, s), direction(glm::normalize(dir)) {}
	glm::vec3 direction;
};

struct Attenuation
{
	GLfloat constant = 1.0;
	GLfloat linear = 0.0;
	GLfloat exp = 1.0;
	int hasshadow = false;
};

struct PointLight
{
	PointLight() : bl(0.0, vec3(1.0), 1.0, 0.2) {}
	PointLight(float i, vec3 c, float d, float s, bool castshadow, vec3 pos, GLfloat co, GLfloat li, GLfloat ex) : bl(i, c, d, s), position(pos)
	{
		atten.constant = co;
		atten.linear = li;
		atten.exp = ex;
		atten.hasshadow = castshadow;
	}
	BaseLight bl;
	glm::vec3 position;
	float not_used_for_alignment = 0.0;
	Attenuation atten;
	float IntenAt(vec3 spot) const
	{
		float x = length(spot - position);
		float attenuation = atten.exp * x * x + atten.linear * x + atten.constant;
		return bl.intensity / attenuation;
	}
};

struct SpotLight
{
	SpotLight() : bl(0.0, glm::vec3(1.0), 1.0, 0.2) {}
	SpotLight(float i, vec3 c, float d, float s, bool castshadow, vec3 pos, vec3 dir, GLfloat co, GLfloat li, GLfloat ex, GLfloat fu, GLfloat ze)
		: bl(i, c, d, s), position(pos), direction(glm::normalize(dir)), fullcos(fu), zerocos(ze)
	{
		atten.constant = co;
		atten.linear = li;
		atten.exp = ex;
		atten.hasshadow = castshadow;
	}
	BaseLight bl;
	glm::vec3 position;
	GLfloat fullcos;
	glm::vec3 direction;
	GLfloat zerocos;
	Attenuation atten;
	float IntenAt(vec3 spot) const
	{
		float x = length(spot - position);
		float attenuation = atten.exp * x * x + atten.linear * x + atten.constant;
		
		vec3 lightdir = normalize(spot - position);
		float lightcos = dot(lightdir, direction);
		if (lightcos > zerocos)
		{
			float spotfactor = 1.0;
			if (lightcos < fullcos)
			{
				spotfactor = (lightcos - zerocos) / (fullcos - zerocos);
			}
			return bl.intensity / attenuation * spotfactor;
		}
		return 0.0f;
	}
};
//Enddef lights
//Light location in shader
struct BaseLightLoc
{
	GLuint intensity;
	GLuint color;
	GLuint diffuse;
	GLuint specular;
};
struct DirectionalLightLoc : BaseLightLoc
{
	GLuint direction;
};
//End light location
//Material defs
struct Material
{
	Material() = default;
	Material(const glm::vec3 & a, const glm::vec3 & d, const glm::vec3 & s, const glm::vec3 & e, GLfloat sh, GLfloat tr )
		: ambientcolor(a), diffusecolor(d), specularcolor(s), emissivecolor(e), shininess(sh), transparency(tr){}
	glm::vec3 ambientcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffusecolor = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 specularcolor = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 emissivecolor = glm::vec3(0.0f, 0.0f ,0.0f);
	GLfloat shininess = 1000.0f;
	float transparency = 1.0f;
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
//Enddef material
//Material location in shader
struct MaterialLocation
{
	GLuint ambientcolor;
	GLuint diffusecolor;
	GLuint specularcolor;
	GLuint emissivecolor;
	GLuint shininess;
	GLuint trans;
};
//End location material

