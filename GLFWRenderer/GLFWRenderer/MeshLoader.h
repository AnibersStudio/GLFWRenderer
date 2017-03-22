#pragma once
#include "Mesh.h"
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
