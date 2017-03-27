#include "DynamicMeshManager.h"

DynamicMeshManager::stub DynamicMeshManager::Add(ArrayModel mesh)
{
	opacelist.insert(opacelist.begin(), ArrayModel());
	fulltranslist.insert(fulltranslist.begin(), ArrayModel());
	semitranslist.insert(semitranslist.begin(), ArrayModel());

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
	}
	if (!(*fulltranslist.begin()).GetMesh().size())
	{
		fulltranslist.erase(opacelist.begin());
	}
	if (!(*semitranslist.begin()).GetMesh().size())
	{
		semitranslist.erase(opacelist.begin());
	}
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

