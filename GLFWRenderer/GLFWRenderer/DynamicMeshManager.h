#pragma once
#include <list>
#include "MeshManager.h"
#include <stack>
struct DynamicMeshStub
{
	unsigned int oentry;
	unsigned int oversion = 0;
	unsigned int fentry;
	unsigned int fversion = 0;
	std::list<std::tuple<std::vector<MaterialedVertex>, glm::vec3>>::iterator tit;
};

struct DynamicMeshEntry
{
	std::vector<MaterialedVertex>* list;
	unsigned int begin;
	unsigned int end;
	bool free;
	unsigned int version = 0;
};

class DynamicMeshLoader
{
public:
	DynamicMeshLoader() { shadermatlist.push_back(ShaderMaterial()); }// 0 is left to default
	bool RegisterMesh(ArrayMesh mesh, std::string path);
	bool RegisterMesh(std::string meshpath, std::string alias = "");
	bool AliasMesh(std::string meshalias, std::string meshname);
	const std::vector<ShaderMaterial>& GetMatList() { return shadermatlist; }
protected:
	using MeshPart = std::tuple<std::vector<MaterialedVertex>, glm::vec3, float>;
	std::vector<MeshPart> ApplyMaterialToMesh(std::string meshname, glm::mat4 transform);
private:
	std::unordered_map<std::string, unsigned int> meshnamemap;
	std::vector<ArrayMesh> meshlist;
	std::vector<std::vector<unsigned int>> matindexlist;
	std::vector<std::vector<glm::vec3>> matposlist;
	std::vector<ShaderMaterial> shadermatlist;
};

class DynamicMeshManager : public MeshManager, public DynamicMeshLoader
{
public:
	//typedef std::tuple<std::list<ArrayMesh>::iterator, std::list<ArrayMesh>::iterator, std::list<ArrayMesh>::iterator> stub;

	//virtual void AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const override;
	//virtual void AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const override;
	//virtual void AppendPosition(unsigned char meshoption, std::vector<glm::vec3> & positionlist) const override;
	//
	///// <summary> Add a mesh to the scene </summary>
	//stub Add(ArrayMesh mesh);
	///// <summary> Delete a mesh of stub returned by Add() </summary>
	//void Delete(stub s);
	virtual const std::vector<ShaderMaterial>& GetShaderMatList() override { return GetMatList(); }
	virtual const std::vector<MaterialedVertex>& GetOpaqueVertex() override { return *firstopaquevertexlist; }
	virtual const std::vector<MaterialedVertex>& GetFulltransVertex() override { return *firstfulltransvertexlist; }
	virtual const std::vector<MaterialedVertex>& GetTransVertex() override { return transvertexlist; }
	virtual const std::vector<unsigned int>& GenerateTransTask(glm::vec3 eye) override;

	using Stub = DynamicMeshStub;
	using Entry = DynamicMeshEntry;
	using VertexList = std::vector<MaterialedVertex>;
	using EntryList = std::vector<Entry>;
	Stub Add(std::string meshname, glm::mat4 transform);
	void Delete(Stub & obj);
	void Clean();
	void SetCleanThreshold(unsigned int threshold) { cleanthreshold = threshold; }
	void SetAutoClean(bool autoclean) { isautoclean = autoclean; }
	void Transform(Stub stub, const glm::mat4& transform);
private:
	void AddVertex(const VertexList& verticestoadd, unsigned int& entrytoadd, unsigned int& version, EntryList& entrylist, std::stack<unsigned int>& freelist, VertexList& vertexlist );
	unsigned int DeleteEntry(unsigned int entrytodel,unsigned int version, EntryList& entrylist, std::stack<unsigned int>& freelist);
	void Clean(EntryList& entrylist, VertexList** firstlist, VertexList** secondlist);
	void Transform(const glm::mat4& transform, unsigned int entrytotransform, unsigned int version, EntryList& entrylist);
	//unsigned int ListVertexCount(const std::list<ArrayMesh> & list) const;
	//unsigned int ReserveVertexSize(unsigned int meshoption) const;
	//unsigned int ListMaterialCount(const std::list<ArrayMesh> & list) const;
	//unsigned int ReserveMaterialSize(unsigned int meshoption) const;
	//std::list<ArrayMesh> opacelist;
	//std::list<ArrayMesh> fulltranslist;
	//std::list<ArrayMesh> semitranslist;
	//bool sorted = false;
	EntryList opaqueentrylist;
	std::stack<unsigned int> opaquefreelist;
	VertexList opaquevertexlist;
	VertexList opaquevertexlist2;
	VertexList* firstopaquevertexlist = &opaquevertexlist;
	VertexList* secondopaquevertexlist = &opaquevertexlist2;
	unsigned int opaquedelcount = 0;

	EntryList fulltransentrylist;
	std::stack<unsigned int> fulltransfreelist;
	VertexList fulltransvertexlist;
	VertexList fulltransvertexlist2;
	VertexList * firstfulltransvertexlist = &fulltransvertexlist;
	VertexList * secondfulltransvertexlist = &fulltransvertexlist2;
	unsigned int fulltransdelcount = 0;

	VertexList transvertexlist;
	std::vector<unsigned int> transtasklist;
	std::list<std::tuple<std::vector<MaterialedVertex>, glm::vec3>> transentitylist;
	unsigned int transvertexcount = 0;

	bool isautoclean = false;
	unsigned int cleanthreshold = 16 * 16 * 4;
};