#pragma comment(lib, "assimp-vc140-mt.lib")
#include "Dependencies/include/assimp/cimport.h"
#include "Mesh.h"
#include "TextureLoader.h"

IndexedMesh::IndexedMesh(const std::string & objpath)
{
	Assimp::Importer objimporter;
	const aiScene * scene = objimporter.ReadFile(objpath, aiProcess_Triangulate);
	objimporter.ApplyPostProcessing(aiProcess_GenNormals | aiProcess_FlipUVs);
	objimporter.ApplyPostProcessing(aiProcess_CalcTangentSpace);
	objimporter.ApplyPostProcessing(aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
	scene = objimporter.GetScene();
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw "Opening model failed.";
	}
	else
	{
		path = objpath.substr(0, objpath.find_last_of("/"));
		ProcessNode(scene->mRootNode, scene);
	}
}

void IndexedMesh::Transform(const glm::mat4 & transformmatrix)
{
	for (auto & vertex : meshv)
	{
		glm::vec4 pos4 = transformmatrix * glm::vec4(vertex.position, 1.0f);
		vertex.position = glm::vec3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);
		glm::vec4 normal = glm::transpose(glm::inverse(transformmatrix)) * glm::vec4(vertex.normal, 1.0);
		vertex.normal = glm::normalize(glm::vec3(normal));
		glm::vec4 tangent = transformmatrix * glm::vec4(vertex.tangent, 1.0f);
		vertex.tangent = glm::normalize(glm::vec3(tangent));
	}
}

void IndexedMesh::ProcessNode(const aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i != node->mNumMeshes; i++)
	{
		ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}
	for (unsigned int i = 0; i != node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

void IndexedMesh::ProcessMesh(const aiMesh * mesh, const aiScene * scene)
{
	unsigned int indexstart = meshv.size();
	if (mesh->mTextureCoords[1])
	{
		return;
	}
	for (unsigned int i = 0; i != mesh->mNumVertices; i++)//Add vertices
	{
		Vertex vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.texcoord = (mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f));
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTangents)
		{
			vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		}
		meshv.push_back(vertex);
	}
	TexturedMaterial material;
	if (mesh->mMaterialIndex >= 0)//Add materials
	{
		aiMaterial * mat = scene->mMaterials[mesh->mMaterialIndex];
		material = ProcessMaterial(mat);
	}
	for (unsigned int i = 0; i != mesh->mNumFaces; i++)//Add indices
	{
		aiFace & face = mesh->mFaces[i];
		for (unsigned int i = 0; i != 3; i++)
		{
			meshi[material].push_back(indexstart + face.mIndices[i]);
		}
	}
}

TexturedMaterial IndexedMesh::ProcessMaterial(const aiMaterial * material)
{
	aiColor4D ambient, diffuse, specular, emissive;
	float shininess, transparency;
	if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) != aiReturn_SUCCESS)
		ambient = aiColor4D(1.0, 1.0, 1.0, 1.0);
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) != aiReturn_SUCCESS)
		diffuse = aiColor4D(0.0, 0.0, 00, 0.0);
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular) != aiReturn_SUCCESS)
		specular = aiColor4D(0.0, 0.0, 00, 0.0);
	if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) != aiReturn_SUCCESS)
		emissive = aiColor4D(0.0, 0.0, 00, 0.0);;
	if (material->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS)
		shininess = 1000.0;
	if (material->Get(AI_MATKEY_OPACITY, transparency) != aiReturn_SUCCESS)
		transparency = 1.0;
	TexturedMaterial texmat(glm::vec3(ambient.r, ambient.g, ambient.b), glm::vec3(diffuse.r, diffuse.g, diffuse.b),
		glm::vec3(specular.r, specular.g, specular.b), glm::vec3(emissive.r, emissive.g, emissive.b), shininess, transparency);
	aiString diffusename, specularname, emissivename, normalname, transname;
	if (material->GetTextureCount(aiTextureType_DIFFUSE))
	{
		material->GetTexture(aiTextureType_DIFFUSE, 0, &diffusename);
		texmat.diffusetex = TextureLoader::GetInstance().Load2DTexture(std::string(path + "/" + std::string(diffusename.C_Str())));//diffuse is sRGB created.
	}
	if (material->GetTextureCount(aiTextureType_SPECULAR))
	{
		material->GetTexture(aiTextureType_SPECULAR, 0, &specularname);
		texmat.speculartex = TextureLoader::GetInstance().Load2DTexture(std::string(path + "/" + std::string(specularname.C_Str())));//specular is sRGB created.
	}
	if (material->GetTextureCount(aiTextureType_EMISSIVE))
	{
		material->GetTexture(aiTextureType_EMISSIVE, 0, &emissivename);
		texmat.emissivetex = TextureLoader::GetInstance().Load2DTexture(std::string(path + "/" + std::string(emissivename.C_Str())));//emissive is sRGB created.
	}
	if (material->GetTextureCount(aiTextureType_HEIGHT))
	{
		material->GetTexture(aiTextureType_HEIGHT, 0, &normalname);
		texmat.normaltex = TextureLoader::GetInstance().Load2DTexture(std::string(path + "/" + std::string(normalname.C_Str())), false);
	}
	if (material->GetTextureCount(aiTextureType_OPACITY))
	{
		material->GetTexture(aiTextureType_OPACITY, 0, &transname);
		texmat.transtex = TextureLoader::GetInstance().Load2DTexture(std::string(path + "/" + std::string(transname.C_Str())));
	}
	return texmat;
}

ArrayMesh::ArrayMesh(const IndexedMesh & im) : path(im.GetPath())
{
	auto & matind = im.GetMeshInd();
	auto & matvert = im.GetMeshVert();
	for (auto & objind : matind)
	{
		const TexturedMaterial & material = objind.first;
		std::vector<unsigned int> indvec = objind.second;
		for (auto ind : indvec)
		{
			mesh[material].push_back(matvert[ind]);
		}
	}
}

void ArrayMesh::Transform(const glm::mat4 & transformmatrix)
{
	for (auto & object : mesh)
	{
		for (auto & vertex : object.second)
		{
			glm::vec4 pos4 = transformmatrix * glm::vec4(vertex.position, 1.0f);
			vertex.position = glm::vec3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);
			glm::vec4 normal = glm::transpose(glm::inverse(transformmatrix)) * glm::vec4(vertex.normal, 1.0);
			vertex.normal = glm::normalize(glm::vec3(normal));
			glm::vec4 tangent = transformmatrix * glm::vec4(vertex.tangent, 1.0f);
			vertex.tangent = glm::normalize(glm::vec3(tangent));
		}
	}

}

const ArrayMesh::Mesh & ArrayMesh::GetMesh() const
{
	return mesh;
}

void ArrayMesh::Add(const TexturedMaterial & material, const MeshVert & verlist)
{
	auto & vertvec = mesh[material];
	vertvec.reserve(vertvec.size() + verlist.size());
	for (auto & vert : verlist)
	{
		vertvec.push_back(vert);
	}
}

unsigned int ArrayMesh::VertexCount()
{
	unsigned int vertexcount = 0;
	for (auto i : mesh)
	{
		vertexcount += i.second.size();
	}
	return vertexcount;
}

ArrayMesh & ArrayMesh::operator+=(const ArrayMesh & rhs)
{
	for (auto & matvecpair : rhs.mesh)
	{
		auto & mat = matvecpair.first;
		auto & vertvec = matvecpair.second;
		Add(mat, vertvec);
	}
	return *this;
}


