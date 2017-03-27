#pragma once
#include <list>

#include "MeshManager.h"

class DynamicMeshManager : public MeshManager
{
public:
	typedef std::tuple<std::list<ArrayModel>::const_iterator, std::list<ArrayModel>::const_iterator, std::list<ArrayModel>::const_iterator> stub;

	virtual std::tuple<std::vector<TexturedMaterial>, std::vector<unsigned int>, std::vector<Vertex>> GetMeshList(unsigned char meshoption) const override;
	virtual std::tuple<std::vector<TexturedMaterial>, std::vector<unsigned int>, std::vector<Vertex>> GetOrderedMeshList(unsigned char meshoption, glm::vec3 eye) const override;
	virtual std::vector<glm::vec3> GetPositionList(unsigned char meshoption) const override;

	virtual void AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> vec, std::vector<unsigned int> count, std::vector<Vertex> vertices) const override;
	virtual void AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> vec, std::vector<unsigned int> count, std::vector<Vertex> vertices) const override;
	virtual void AppendPosition(unsigned char meshoption, std::vector<glm::vec3> positionlist) const override;

	stub Add(ArrayModel mesh);
	void Delete(stub s);
private:
	std::list<ArrayModel> opacelist;
	std::list<ArrayModel> fulltranslist;
	std::list<ArrayModel> semitranslist;
};