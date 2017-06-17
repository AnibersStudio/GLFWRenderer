#include "DynamicMeshManager.h"
//
//void DynamicMeshManager::AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const
//{
//	count.reserve(count.size() + ReserveMaterialSize(meshoption));
//	vertices.reserve(vertices.size() + ReserveVertexSize(meshoption));
//	materialindex.reserve(materialindex.size() + ReserveMaterialSize(meshoption));
//
//	std::unordered_map<TexturedMaterial, unsigned int> materialmap;
//	materialmap.reserve(ReserveMaterialSize(Opace) * 2);
//	unsigned int currentindex;
//	if (meshoption & Opace)
//	{
//		for (auto & m : opacelist)
//		{
//			for (auto & matvec : m.GetMesh())
//			{
//				auto matit = materialmap.find(matvec.first);
//				if (matit != materialmap.end())
//				{
//					currentindex = matit->second;
//				}
//				else
//				{
//					currentindex = materialmap.size();
//					materialmap[matvec.first] = currentindex;
//				}
//				count.push_back(matvec.second.size());
//				for (auto & v : matvec.second)
//				{
//					vertices.push_back(v);
//				}
//			}
//		}
//	}
//	if (meshoption& FullTrans)
//	{
//		for (auto & m : fulltranslist)
//		{
//			for (auto & matvec : m.GetMesh())
//			{
//				auto matit = materialmap.find(matvec.first);
//				if (matit != materialmap.end())
//				{
//					currentindex = matit->second;
//				}
//				else
//				{
//					currentindex = materialmap.size();
//					materialmap[matvec.first] = currentindex;
//				}
//				count.push_back(matvec.second.size());
//				for (auto & v : matvec.second)
//				{
//					vertices.push_back(v);
//				}
//			}
//		}
//	}
//	if (meshoption & SemiTrans)
//	{
//		for (auto & m : semitranslist)
//		{
//			for (auto & matvec : m.GetMesh())
//			{
//				auto matit = materialmap.find(matvec.first);
//				if (matit != materialmap.end())
//				{
//					currentindex = matit->second;
//				}
//				else
//				{
//					currentindex = materialmap.size();
//					materialmap[matvec.first] = currentindex;
//				}
//				count.push_back(matvec.second.size());
//				for (auto & v : matvec.second)
//				{
//					vertices.push_back(v);
//				}
//			}
//		}
//	}
//
//	unsigned int oldsize = mat.size();
//	mat.resize(mat.size() + materialmap.size());
//	for (auto & m : materialmap)
//	{
//		mat[oldsize + m.second] = std::move(m.first);
//	}
//}
//
//void DynamicMeshManager::AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const
//{
//	mat.reserve(mat.size() + ReserveMaterialSize(meshoption));
//	count.reserve(count.size() + ReserveMaterialSize(meshoption));
//	vertices.reserve(vertices.size() + ReserveVertexSize(meshoption));
//
//	std::list<ArrayMesh> meshlist;
//	if (meshoption & Opace)
//	{
//		for (auto & m : opacelist)
//		{
//			meshlist.push_back(m);
//		}
//	}
//	if (meshoption & FullTrans)
//	{
//		for (auto & m : fulltranslist)
//		{
//			meshlist.push_back(m);
//		}
//	}
//	if (meshoption & SemiTrans)
//	{
//		for (auto & m : semitranslist)
//		{
//			meshlist.push_back(m);
//		}
//	}
//	meshlist.sort([eye](const ArrayMesh & lhs, const ArrayMesh & rhs) { return glm::length(lhs.position - eye) >= glm::length(rhs.position - eye); });
//
//	std::unordered_map<TexturedMaterial, unsigned int> materialmap;
//	materialmap.reserve(ReserveMaterialSize(Opace) * 2);
//	unsigned int currentindex;
//	for (auto & m : meshlist)
//	{
//		for (auto & matvec : m.GetMesh())
//		{
//			auto matit = materialmap.find(matvec.first);
//			if (matit != materialmap.end())
//			{
//				currentindex = matit->second;
//			}
//			else
//			{
//				currentindex = materialmap.size();
//				materialmap[matvec.first] = currentindex;
//			}
//			count.push_back(matvec.second.size());
//			for (auto & v : matvec.second)
//			{
//				vertices.push_back(v);
//			}
//		}
//	}
//
//	unsigned int oldsize = mat.size();
//	mat.resize(mat.size() + materialmap.size());
//	for (auto & m : materialmap)
//	{
//		mat[oldsize + m.second] = std::move(m.first);
//	}
//}
//
//void DynamicMeshManager::AppendPosition(unsigned char meshoption, std::vector<glm::vec3> & positionlist) const
//{
//	positionlist.reserve(positionlist.size() + ReserveVertexSize(meshoption));
//	if (meshoption & Opace)
//	{
//		for (auto & m : opacelist)
//		{
//			for (auto & matvec : m.GetMesh())
//			{
//				for (auto & v : matvec.second)
//				{
//					positionlist.push_back(v.position);
//				}
//			}
//		}
//	}
//	if (meshoption& FullTrans)
//	{
//		for (auto & m : fulltranslist)
//		{
//			for (auto & matvec : m.GetMesh())
//			{
//				for (auto & v : matvec.second)
//				{
//					positionlist.push_back(v.position);
//				}
//			}
//		}
//	}
//	if (meshoption & SemiTrans)
//	{
//		for (auto & m : semitranslist)
//		{
//			for (auto & matvec : m.GetMesh())
//			{
//				for (auto & v : matvec.second)
//				{
//					positionlist.push_back(v.position);
//				}
//			}
//		}
//	}
//}
//
//DynamicMeshManager::stub DynamicMeshManager::Add(ArrayMesh mesh)
//{
//	opacelist.insert(opacelist.begin(), ArrayMesh());
//	fulltranslist.insert(fulltranslist.begin(), ArrayMesh());
//	semitranslist.insert(semitranslist.begin(), ArrayMesh());
//
//	auto it1 = opacelist.begin(), it2 = fulltranslist.begin(), it3 = semitranslist.begin();
//
//	for (auto & m : mesh.GetMesh())
//	{
//		if (m.first.transparency > 0.99999)
//		{
//			(*opacelist.begin()).Add(m.first, m.second);
//		}
//		else if (m.first.transparency < 0.00001)
//		{
//			(*fulltranslist.begin()).Add(m.first, m.second);
//		}
//		else
//		{
//			(*semitranslist.begin()).Add(m.first, m.second);
//		}
//	}
//	if (!(*opacelist.begin()).GetMesh().size())
//	{
//		opacelist.erase(opacelist.begin());
//		it1 = opacelist.end();
//	}
//	if (!(*fulltranslist.begin()).GetMesh().size())
//	{
//		fulltranslist.erase(fulltranslist.begin());
//		it2 = fulltranslist.end();
//	}
//	if (!(*semitranslist.begin()).GetMesh().size())
//	{
//		semitranslist.erase(semitranslist.begin());
//		it3 = semitranslist.end();
//	}
//	return stub(it1, it2, it3);
//}
//
//void DynamicMeshManager::Delete(stub s)
//{
//	auto a = std::get<0>(s);
//	auto b = std::get<1>(s);
//	auto c = std::get<2>(s);
//	if (a != opacelist.end())
//	{
//		opacelist.erase(a);
//	}
//	if (b != fulltranslist.end())
//	{
//		fulltranslist.erase(b);
//	}
//	if (c != semitranslist.end())
//	{
//		semitranslist.erase(c);
//	}
//}
//
//unsigned int DynamicMeshManager::ListVertexCount(const std::list<ArrayMesh>& list) const
//{
//	unsigned int vertcount = 0;
//	for (auto& m : list)
//	{
//		for (auto& v : m.GetMesh())
//		{
//			vertcount += v.second.size();
//		}
//	}
//	return vertcount;
//}
//
//unsigned int DynamicMeshManager::ReserveVertexSize(unsigned int meshoption) const
//{
//	unsigned int size = 0;
//	if (meshoption & Opace)
//	{
//		size += ListVertexCount(opacelist);
//	}
//	if (meshoption & FullTrans)
//	{
//		size += ListVertexCount(fulltranslist);
//	}
//	if (meshoption & SemiTrans)
//	{
//		size += ListVertexCount(semitranslist);
//	}
//	return size;
//}
//
//unsigned int DynamicMeshManager::ListMaterialCount(const std::list<ArrayMesh>& list) const
//{
//	unsigned int matcount = 0;
//	for (auto & m : list)
//	{
//		matcount += m.GetMesh().size();
//	}
//	return matcount;
//}
//
//unsigned int DynamicMeshManager::ReserveMaterialSize(unsigned int meshoption) const
//{
//	unsigned int size = 0;
//	if (meshoption & Opace)
//	{
//		size += ListMaterialCount(opacelist);
//	}
//	if (meshoption & FullTrans)
//	{
//		size += ListMaterialCount(fulltranslist);
//	}
//	if (meshoption & SemiTrans)
//	{
//		size += ListMaterialCount(semitranslist);
//	}
//	return size;
//}

bool DynamicMeshLoader::RegisterMesh(ArrayMesh mesh, std::string path)
{
	// Already exist. Return error
	if (meshnamemap.find(path) != meshnamemap.end())
	{
		return false;
	}
	// Set Path as a name(alias)
	meshnamemap[path] = meshlist.size();

	std::vector<unsigned int> matindex;
	std::vector<glm::vec3> matpos;
	for (auto& matvec : mesh.GetMesh())
	{
		{// Set Shader Texture List
			const TexturedMaterial & material = matvec.first;
			ShaderMaterial sm{ material };
			auto SetTextureHandle = [](ShaderTexture & st, Texture2D * texture) { if (texture) { st.is = true; st.handle = texture->GetObjectHandle(); } };
			SetTextureHandle(sm.diffuse, material.diffusetex);
			SetTextureHandle(sm.specular, material.speculartex);
			SetTextureHandle(sm.emissive, material.emissivetex);
			SetTextureHandle(sm.normal, material.normaltex);
			SetTextureHandle(sm.trans, material.transtex);

			matindex.push_back(shadermatlist.size());
			shadermatlist.push_back(sm);
		}
		{// Set Position List
			const std::vector<Vertex> & vertlist = matvec.second;
			glm::vec3 possum(0.0f);
			for (auto & vert : vertlist)
			{
				possum += vert.position;
			}
			possum /= vertlist.size();

			matpos.push_back(possum);
		}
	}
	// Add mesh to list
	meshlist.push_back(std::move(mesh));
	// Add matindex to list
	matindexlist.push_back(std::move(matindex));
	// Add matpos to lsit
	matposlist.push_back(std::move(matpos));
	return true;
}

bool DynamicMeshLoader::RegisterMesh(std::string meshpath, std::string alias)
{
	bool succeed = RegisterMesh(ArrayMesh(IndexedMesh(meshpath)), meshpath);
	if (alias != "")
	{
		succeed = AliasMesh(alias, meshpath) && succeed;
	}
	return succeed;
}

bool DynamicMeshLoader::AliasMesh(std::string meshalias, std::string meshname)
{
	if (meshnamemap.find(meshalias) != meshnamemap.end())
	{
		return false;
	}
	else
	{
		meshnamemap[meshalias] = meshnamemap[meshname];
		return true;
	}
}

std::vector<DynamicMeshLoader::MeshPart> DynamicMeshLoader::ApplyMaterialToMesh(std::string meshname, glm::mat4 transform)
{
	std::vector<std::tuple<std::vector<MaterialedVertex>, glm::vec3, float>> res;

	// Not exist, return empty
	if (meshnamemap.find(meshname) == meshnamemap.end())
	{
		return res;
	}
	
	unsigned int meshindex = meshnamemap[meshname];
	ArrayMesh & mesh = meshlist[meshindex];
	std::vector<glm::vec3> & matpos = matposlist[meshindex];
	std::vector<unsigned int> & matindex = matindexlist[meshindex];

	int i = 0;
	for (auto it = mesh.GetMesh().begin(); it != mesh.GetMesh().end(); ++it, ++i)
	{
		std::vector<MaterialedVertex> vertexlist;
		vertexlist.reserve(it->second.size());
		unsigned int index = matindex[i];

		float trans = it->first.transparency;
		glm::vec4 pos4 = transform * glm::vec4(matpos[i], 1.0f);
		glm::vec3 pos = glm::vec3(pos4) / pos4.w;
		
		for (auto vert : it->second)
		{
			vert.Transform(transform);
			vertexlist.push_back(MaterialedVertex{ vert, index + 1 });
		}
		res.push_back(MeshPart(std::move(vertexlist), pos, trans));
	}
	return std::move(res);
}

const std::vector<unsigned int>& DynamicMeshManager::GenerateTransTask(glm::vec3 eye)
{
	transvertexlist.resize(0);
	transvertexlist.reserve(transvertexcount);
	transtasklist.resize(0);
	transtasklist.reserve(transvertexcount);

	auto comparer = [eye](const std::tuple<std::vector<MaterialedVertex>, glm::vec3> & lhs, const std::tuple<std::vector<MaterialedVertex>, glm::vec3> & rhs) { return glm::length(std::get<1>(lhs) - eye) >= glm::length(std::get<1>(rhs) - eye); };
	transentitylist.sort(comparer);
	for (auto i : transentitylist)
	{
		const std::vector<MaterialedVertex>& vec = std::get<0>(i);
		for (auto& v : vec)
		{
			transvertexlist.emplace_back(v);
		}
		transtasklist.push_back(vec.size());
	}
	return transtasklist;
}

DynamicMeshManager::Stub DynamicMeshManager::Add(std::string meshname, glm::mat4 transform)
{
	Stub stub;
	auto addition = ApplyMaterialToMesh(meshname, transform);
	for (auto& t : addition)
	{
		VertexList& vlist = std::get<0>(t);
		glm::vec3 pos = std::get<1>(t);
		float trans = std::get<2>(t);

		if (trans > 1 - 0.00001)//Opaque
		{
			AddVertex(vlist, stub.oentry, stub.oversion, opaqueentrylist, opaquefreelist, *firstopaquevertexlist);
		}
		else if (trans < 0.00001)//Full trans
		{
			AddVertex(vlist, stub.fentry, stub.fversion, fulltransentrylist, fulltransfreelist, *firstfulltransvertexlist);
		}
		else //Trans
		{
			transentitylist.emplace_back(std::move(vlist), pos );
			transvertexcount += vlist.size();
			stub.tit = transentitylist.end()--;
		}
	}
	return stub;
}

void DynamicMeshManager::Delete(Stub & obj)
{
	opaquedelcount += DeleteEntry(obj.oentry, obj.oversion, opaqueentrylist, opaquefreelist);
	fulltransdelcount += DeleteEntry(obj.fentry, obj.fversion, fulltransentrylist, fulltransfreelist);
	transvertexcount -= std::get<0>(*obj.tit).size();

	if (isautoclean)
	{
		Clean();
	}
}

void DynamicMeshManager::Clean()
{
	if (opaquedelcount > cleanthreshold)
	{
		Clean(opaqueentrylist, &firstopaquevertexlist, &secondopaquevertexlist);
		opaquedelcount = 0u;
	}
	if (fulltransdelcount > cleanthreshold)
	{
		Clean(fulltransentrylist, &firstfulltransvertexlist, &secondfulltransvertexlist);
		fulltransdelcount = 0u;
	}
}


void DynamicMeshManager::Transform(Stub stub, const glm::mat4& transform)
{
	Transform(transform, stub.oentry, stub.oversion, opaqueentrylist);
	Transform(transform, stub.fentry, stub.fversion, fulltransentrylist);
	auto& objnode = *stub.tit;
	auto& vec = std::get<0>(objnode);
	glm::vec3& pos = std::get<1>(objnode);
	for (auto& i : vec)
	{
		i.vertex.Transform(transform);
	}
	glm::vec4 pos4 = transform * glm::vec4(pos, 1.0f);
	pos = glm::vec3(pos4) / pos4.w;
}

void DynamicMeshManager::AddVertex(const VertexList & verticestoadd, unsigned int & entrytoadd, unsigned int & version, EntryList & entrylist, std::stack<unsigned int>& freelist, VertexList & vertexlist)
{
	unsigned int entryindex;
	if (freelist.empty())// Get a free or alloc a entry
	{
		entryindex = entrylist.size();
		entrylist.emplace_back();
	}
	else
	{
		entryindex = freelist.top();
		freelist.pop();
	}

	entrytoadd = entryindex;// Assign output
	version = entrylist[entrytoadd].version;

	Entry& entry = entrylist[entrytoadd];// Edit entry
	entry.list = &vertexlist;
	entry.begin = vertexlist.size();
	entry.end = vertexlist.size() + verticestoadd.size();
	entry.free = false;
	
	vertexlist.reserve(vertexlist.size() + verticestoadd.size());
	for (auto & v : verticestoadd)// Insert vertices
	{
		vertexlist.emplace_back(v);
	}
}

unsigned int DynamicMeshManager::DeleteEntry(unsigned int entrytodel, unsigned int version, EntryList & entrylist, std::stack<unsigned int>& freelist)
{
	if (entrylist[entrytodel].version == version && entrylist[entrytodel].free == false)// Prevent double delete
	{
		entrylist[entrytodel].free = true;
		entrylist[entrytodel].version++;
		freelist.push(entrytodel);
		auto& vec = *entrylist[entrytodel].list;
		for (auto i = entrylist[entrytodel].begin; i != entrylist[entrytodel].end; i++)
		{
			vec[i].material = 0u;
			vec[i].vertex.position = glm::vec3(0.0f);
		}
		return entrylist[entrytodel].end - entrylist[entrytodel].begin;
	}
	return 0u;
}

void DynamicMeshManager::Clean(EntryList & entrylist, VertexList** firstlist, VertexList** secondlist)
{
	VertexList& firstvertexlist = **firstlist, secondvertexlist = **secondlist;
	for (auto e : entrylist)
	{
		if (!e.free)
		{
			e.list = *secondlist;
			e.end = secondvertexlist.size() + e.end - e.begin;
			e.begin = secondvertexlist.size();
			for (unsigned int i = e.begin; i != e.end; i++)
			{
				secondvertexlist.push_back(firstvertexlist[i]);
			}
		}
	}
	auto temp = *firstlist;
	(*firstlist) = (*secondlist);
	(*secondlist) = temp;
}

void DynamicMeshManager::Transform(const glm::mat4& transform, unsigned int entrytotransform, unsigned int version, EntryList & entrylist)
{
	if (entrylist[entrytotransform].version == version && entrylist[entrytotransform].free == false)
	{
		auto& entry = entrylist[entrytotransform];
		auto& vertexlist = *entry.list;
		for (unsigned int i = entry.begin; i != entry.end; i++)
		{
			vertexlist[i].vertex.Transform(transform);
		}
	}
}
