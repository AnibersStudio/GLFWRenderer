//#pragma comment(lib, "zlibstatic.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "Model.h"
#include <assimp/cimport.h>
IndexedModel::IndexedModel(const std::string & objpath, bool isantialias, bool isshapesmooth) : iscolorsmooth(isantialias), isnormalsmooth(isshapesmooth)
{
	Assimp::Importer objimporter;
	const aiScene * scene = objimporter.ReadFile(objpath, aiProcess_Triangulate );
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

void IndexedModel::Transform(const glm::mat4 & transformmatrix)
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

void IndexedModel::ProcessNode(const aiNode * node, const aiScene * scene)
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

void IndexedModel::ProcessMesh(const aiMesh * mesh, const aiScene * scene)
{
	unsigned int indexstart = meshv.size();
	if (mesh->mTextureCoords[1])
	{
		return;
	}
	for (unsigned int i = 0; i != mesh->mNumVertices; i++)//Add vertices
	{
		Vertex vertex;
		vertex.SetPosition(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		vertex.SetTexcoord(mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f));
		vertex.SetNormal(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		if (mesh->mTangents)
		{
			vertex.SetTangent(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
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

TexturedMaterial IndexedModel::ProcessMaterial(const aiMaterial * material)
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
		texmat.diffusetex = TextureLoader::Load2DTexture( path + "/" + std::string(diffusename.C_Str()), iscolorsmooth, true, true);//diffuse is sRGB created.
	}
	if (material->GetTextureCount(aiTextureType_SPECULAR))
	{
		material->GetTexture(aiTextureType_SPECULAR, 0, &specularname);
		texmat.speculartex = TextureLoader::Load2DTexture(path + "/" + std::string(specularname.C_Str()), iscolorsmooth);
	}
	if (material->GetTextureCount(aiTextureType_EMISSIVE))
	{
		material->GetTexture(aiTextureType_EMISSIVE, 0, &emissivename);
		texmat.emissivetex = TextureLoader::Load2DTexture(path + "/" + std::string(emissivename.C_Str()), iscolorsmooth, true, true);//emissive is sRGB created.
	}
	if (material->GetTextureCount(aiTextureType_HEIGHT))
	{
		material->GetTexture(aiTextureType_HEIGHT, 0, &normalname);
		texmat.normaltex = TextureLoader::Load2DTexture(path + "/" + std::string(normalname.C_Str()), isnormalsmooth, false);
	}
	if (material->GetTextureCount(aiTextureType_OPACITY))
	{
		material->GetTexture(aiTextureType_OPACITY, 0, &transname);
		texmat.transtex = TextureLoader::Load2DTexture(path + "/" + std::string(transname.C_Str()), false, false, true);
	}
	return texmat;
}

ArrayModel::ArrayModel(const IndexedModel & im): path(im.GetPath())
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

void ArrayModel::Transform(const glm::mat4 & transformmatrix)
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

const ArrayModel::Mesh & ArrayModel::GetMesh() const
{
	return mesh;
}

void ArrayModel::Add(const TexturedMaterial & material, const MeshVert & verlist)
{
	for (auto & vert : verlist)
	{
		mesh[material].push_back(vert);
	}
}

ArrayModel & ArrayModel::operator+=(const ArrayModel & rhs)
{
	for (auto & matvecpair : rhs.mesh)
	{
		auto & mat = matvecpair.first;
		auto & vertvec = matvecpair.second;
		Add(mat, vertvec);
	}
	return *this;
}

std::unordered_map <const std::tuple<std::string, bool, bool>, const Texture2D * > TextureLoader::texmap;

const Texture2D * TextureLoader::Load2DTexture(std::string & path, bool issmooth, bool is_linear_to_distance, bool issRGB)
{
	try 
	{
		texmap.at(std::tuple<std::string, bool, bool>(path, issmooth, is_linear_to_distance));
	}
	catch (const std::out_of_range&)
	{
		texmap[std::tuple<std::string, bool, bool>(path, issmooth, is_linear_to_distance)] = new Texture2D(GL_TEXTURE_2D, path, issmooth, is_linear_to_distance, issRGB);
	}
	return texmap[std::tuple<std::string, bool, bool>(path, issmooth, is_linear_to_distance)];
}
