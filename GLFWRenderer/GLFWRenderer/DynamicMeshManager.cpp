#include "DynamicMeshManager.h"

void DynamicMeshManager::AppendMesh(unsigned char meshoption, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const
{
	count.reserve(count.size() + ReserveMaterialSize(meshoption));
	vertices.reserve(vertices.size() + ReserveVertexSize(meshoption));
	materialindex.reserve(materialindex.size() + ReserveMaterialSize(meshoption));

	std::unordered_map<TexturedMaterial, unsigned int> materialmap;
	materialmap.reserve(ReserveMaterialSize(Opace) * 2);
	unsigned int currentindex;
	if (meshoption & Opace)
	{
		for (auto & m : opacelist)
		{
			for (auto & matvec : m.GetMesh())
			{
				auto matit = materialmap.find(matvec.first);
				if (matit != materialmap.end())
				{
					currentindex = matit->second;
				}
				else
				{
					currentindex = materialmap.size();
					materialmap[matvec.first] = currentindex;
				}
				count.push_back(matvec.second.size());
				for (auto & v : matvec.second)
				{
					vertices.push_back(v);
				}
			}
		}
	}
	if (meshoption& FullTrans)
	{
		for (auto & m : fulltranslist)
		{
			for (auto & matvec : m.GetMesh())
			{
				auto matit = materialmap.find(matvec.first);
				if (matit != materialmap.end())
				{
					currentindex = matit->second;
				}
				else
				{
					currentindex = materialmap.size();
					materialmap[matvec.first] = currentindex;
				}
				count.push_back(matvec.second.size());
				for (auto & v : matvec.second)
				{
					vertices.push_back(v);
				}
			}
		}
	}
	if (meshoption & SemiTrans)
	{
		for (auto & m : semitranslist)
		{
			for (auto & matvec : m.GetMesh())
			{
				auto matit = materialmap.find(matvec.first);
				if (matit != materialmap.end())
				{
					currentindex = matit->second;
				}
				else
				{
					currentindex = materialmap.size();
					materialmap[matvec.first] = currentindex;
				}
				count.push_back(matvec.second.size());
				for (auto & v : matvec.second)
				{
					vertices.push_back(v);
				}
			}
		}
	}

	unsigned int oldsize = mat.size();
	mat.resize(mat.size() + materialmap.size());
	for (auto & m : materialmap)
	{
		mat[oldsize + m.second] = std::move(m.first);
	}
}

void DynamicMeshManager::AppendOrderedMesh(unsigned char meshoption, glm::vec3 eye, std::vector<TexturedMaterial> & mat, std::vector<unsigned int> & count, std::vector<Vertex> & vertices, std::vector<unsigned int> & materialindex) const
{
	mat.reserve(mat.size() + ReserveMaterialSize(meshoption));
	count.reserve(count.size() + ReserveMaterialSize(meshoption));
	vertices.reserve(vertices.size() + ReserveVertexSize(meshoption));

	std::list<ArrayModel> meshlist;
	if (meshoption & Opace)
	{
		for (auto & m : opacelist)
		{
			meshlist.push_back(m);
		}
	}
	if (meshoption & FullTrans)
	{
		for (auto & m : fulltranslist)
		{
			meshlist.push_back(m);
		}
	}
	if (meshoption & SemiTrans)
	{
		for (auto & m : semitranslist)
		{
			meshlist.push_back(m);
		}
	}
	meshlist.sort([eye](const ArrayModel & lhs, const ArrayModel & rhs) { return glm::length(lhs.position - eye) >= glm::length(rhs.position - eye); });

	std::unordered_map<TexturedMaterial, unsigned int> materialmap;
	materialmap.reserve(ReserveMaterialSize(Opace) * 2);
	unsigned int currentindex;
	for (auto & m : meshlist)
	{
		for (auto & matvec : m.GetMesh())
		{
			auto matit = materialmap.find(matvec.first);
			if (matit != materialmap.end())
			{
				currentindex = matit->second;
			}
			else
			{
				currentindex = materialmap.size();
				materialmap[matvec.first] = currentindex;
			}
			count.push_back(matvec.second.size());
			for (auto & v : matvec.second)
			{
				vertices.push_back(v);
			}
		}
	}

	unsigned int oldsize = mat.size();
	mat.resize(mat.size() + materialmap.size());
	for (auto & m : materialmap)
	{
		mat[oldsize + m.second] = std::move(m.first);
	}
}

void DynamicMeshManager::AppendPosition(unsigned char meshoption, std::vector<glm::vec3> & positionlist) const
{
	positionlist.reserve(positionlist.size() + ReserveVertexSize(meshoption));
	if (meshoption & Opace)
	{
		for (auto & m : opacelist)
		{
			for (auto & matvec : m.GetMesh())
			{
				for (auto & v : matvec.second)
				{
					positionlist.push_back(v.position);
				}
			}
		}
	}
	if (meshoption& FullTrans)
	{
		for (auto & m : fulltranslist)
		{
			for (auto & matvec : m.GetMesh())
			{
				for (auto & v : matvec.second)
				{
					positionlist.push_back(v.position);
				}
			}
		}
	}
	if (meshoption & SemiTrans)
	{
		for (auto & m : semitranslist)
		{
			for (auto & matvec : m.GetMesh())
			{
				for (auto & v : matvec.second)
				{
					positionlist.push_back(v.position);
				}
			}
		}
	}
}

DynamicMeshManager::stub DynamicMeshManager::Add(ArrayModel mesh)
{
	opacelist.insert(opacelist.begin(), ArrayModel());
	fulltranslist.insert(fulltranslist.begin(), ArrayModel());
	semitranslist.insert(semitranslist.begin(), ArrayModel());

	auto it1 = opacelist.begin(), it2 = fulltranslist.begin(), it3 = semitranslist.begin();

	for (auto & m : mesh.GetMesh())
	{
		if (m.first.transparency > 0.99999)
		{
			(*opacelist.begin()).Add(m.first, m.second);
		}
		else if (m.first.transparency < 0.00001)
		{
			(*fulltranslist.begin()).Add(m.first, m.second);
		}
		else
		{
			(*semitranslist.begin()).Add(m.first, m.second);
		}
	}
	if (!(*opacelist.begin()).GetMesh().size())
	{
		opacelist.erase(opacelist.begin());
		it1 = opacelist.end();
	}
	if (!(*fulltranslist.begin()).GetMesh().size())
	{
		fulltranslist.erase(fulltranslist.begin());
		it2 = fulltranslist.end();
	}
	if (!(*semitranslist.begin()).GetMesh().size())
	{
		semitranslist.erase(semitranslist.begin());
		it3 = semitranslist.end();
	}
	return stub(it1, it2, it3);
}

void DynamicMeshManager::Delete(stub s)
{
	auto a = std::get<0>(s);
	auto b = std::get<1>(s);
	auto c = std::get<2>(s);
	if (a != opacelist.end())
	{
		opacelist.erase(a);
	}
	if (b != fulltranslist.end())
	{
		fulltranslist.erase(b);
	}
	if (c != semitranslist.end())
	{
		semitranslist.erase(c);
	}
}

unsigned int DynamicMeshManager::ListVertexCount(const std::list<ArrayModel>& list) const
{
	unsigned int vertcount = 0;
	for (auto& m : list)
	{
		for (auto& v : m.GetMesh())
		{
			vertcount += v.second.size();
		}
	}
	return vertcount;
}

unsigned int DynamicMeshManager::ReserveVertexSize(unsigned int meshoption) const
{
	unsigned int size = 0;
	if (meshoption & Opace)
	{
		size += ListVertexCount(opacelist);
	}
	if (meshoption & FullTrans)
	{
		size += ListVertexCount(fulltranslist);
	}
	if (meshoption & SemiTrans)
	{
		size += ListVertexCount(semitranslist);
	}
	return size;
}

unsigned int DynamicMeshManager::ListMaterialCount(const std::list<ArrayModel>& list) const
{
	unsigned int matcount = 0;
	for (auto & m : list)
	{
		matcount += m.GetMesh().size();
	}
	return matcount;
}

unsigned int DynamicMeshManager::ReserveMaterialSize(unsigned int meshoption) const
{
	unsigned int size = 0;
	if (meshoption & Opace)
	{
		size += ListMaterialCount(opacelist);
	}
	if (meshoption & FullTrans)
	{
		size += ListMaterialCount(fulltranslist);
	}
	if (meshoption & SemiTrans)
	{
		size += ListMaterialCount(semitranslist);
	}
	return size;
}

