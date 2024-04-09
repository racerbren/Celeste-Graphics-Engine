#include "AssimpImport.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

Mesh3D fromAssimpMesh(const aiMesh* mesh, const std::vector<Map> maps) 
{	
	std::vector<Vertex3D> vertices;
	// Construct the vertices of the mesh and corrresponding texture coordinates
	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		auto& meshVertex = mesh->mVertices[i];
		auto& meshNormal = mesh->mNormals[i];
		auto& texCoord = mesh->mTextureCoords[0][i];
		auto vertex = Vertex3D(glm::vec3(meshVertex.x, meshVertex.y, meshVertex.z), glm::vec3(meshNormal.x, meshNormal.y, meshNormal.z), glm::vec2(texCoord.x, texCoord.y));
		vertices.push_back(vertex);
	}

	std::vector<uint32_t> faces;
	// Construct the faces of the mesh
	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		auto& meshFace = mesh->mFaces[i];
		faces.push_back(meshFace.mIndices[0]);
		faces.push_back(meshFace.mIndices[1]);
		faces.push_back(meshFace.mIndices[2]);
	}

	return Mesh3D(vertices, faces, maps);
}

Object3D assimpLoad(const std::string& path, bool flipTextureCoords, bool genNormals, bool genUV)
{
	Assimp::Importer importer;

	auto options = aiProcessPreset_TargetRealtime_MaxQuality;
	if (flipTextureCoords) {
		options |= aiProcess_FlipUVs;
	}

	if (genNormals) {
		options |= aiProcess_GenNormals;
	}

	if (genUV) {
		options |= aiProcess_GenUVCoords;
	}
	const aiScene* scene = importer.ReadFile(path, options);

	// If the import failed, report it
	if (nullptr == scene) {
		throw std::runtime_error("Error loading assimp file ");

	}
	else {

	}
	auto* mesh = scene->mMeshes[0];
	Map map;
	std::vector<Map> maps;
	uint32_t ID;
	glGenTextures(1, &ID);

	if (mesh->mMaterialIndex >= 0)
	{
		// Locate the "diffuse map" of the mesh, which is the mesh's primary texture.
		auto* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		material->GetTexture(aiTextureType_DIFFUSE, 0, &name);

		// Locate and load the texture image into RAM.
		std::filesystem::path modelPath = path;
		std::filesystem::path texPath = modelPath.parent_path() / name.C_Str();

		map.id = ID;
		map.path = texPath.string();
		map.type = "diffuse";
		map.texture = IMG_Load(texPath.string().c_str());
		if (!map.texture)
			std::cout << "failed to load diffuse map" << SDL_GetError() << "\n";
		else
			std::cout << "loaded diffuse map successfully";

		maps.push_back(map);
	}

	if (!map.texture)
	{
		map.id = ID;
		map.path = "resources/error.jpg";
		map.type = "texture";
		map.texture = IMG_Load(map.path.c_str());
		if (!map.texture)
			std::cout << "failed to load texture" << SDL_GetError() << "\n";
		else
			std::cout << "loaded texture successfully";

		maps.push_back(map);
	}	
	auto ret = Object3D(std::make_shared<Mesh3D>(fromAssimpMesh(scene->mMeshes[0], maps)));
	return ret;
}
