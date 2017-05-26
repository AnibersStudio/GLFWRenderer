#pragma once
#include <list>

#include "MeshManager.h"

class DynamicMeshManager : public MeshManager
{
public:
	typedef std::tuple<std::list<ArrayModel>::iterator, std::list<ArrayModel>::iterator, std::list<ArrayModel>::iterator> stub;

	virtual void AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const override;
	virtual void AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const override;
	virtual void AppendPosition(unsigned char meshoption, std::vector<glm::vec3> & positionlist) const override;
	
	/// <summary> Add a mesh to the scene </summary>
	stub Add(ArrayModel mesh);
	/// <summary> Delete a mesh of stub returned by Add() </summary>
	void Delete(stub s);
private:
	unsigned int ListVertexCount(const std::list<ArrayModel> & list) const;
	unsigned int ReserveVertexSize(unsigned int meshoption) const;
	unsigned int ListMaterialCount(const std::list<ArrayModel> & list) const;
	unsigned int ReserveMaterialSize(unsigned int meshoption) const;
	std::list<ArrayModel> opacelist;
	std::list<ArrayModel> fulltranslist;
	std::list<ArrayModel> semitranslist;
	bool sorted = false;
};