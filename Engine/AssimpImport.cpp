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

	std::vector<Object3D> meshes;
	std::cout << scene->mNumMeshes << "\n";

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		auto* mesh = scene->mMeshes[i];
		std::vector<Map> maps;

		if (mesh->mMaterialIndex >= 0)
		{
			// Locate the "diffuse map" of the mesh, which is the mesh's primary texture.
			auto* material = scene->mMaterials[mesh->mMaterialIndex];
			
			std::vector<Map> diffuseMaps = loadLightingMaps(material, aiTextureType_DIFFUSE, "diffuse", path);
			maps.insert(maps.end(), diffuseMaps.begin(), diffuseMaps.end());
			
			std::vector<Map> specularMaps = loadLightingMaps(material, aiTextureType_SPECULAR, "specular", path);
			maps.insert(maps.end(), specularMaps.begin(), specularMaps.end());

			std::vector<Map> normalMaps = loadLightingMaps(material, aiTextureType_NORMALS, "normal", path);
			maps.insert(maps.end(), normalMaps.begin(), normalMaps.end());

			std::cout << mesh->mName.C_Str() << " maps: " << maps.size() << "\n";
		}

		/*if (!map.texture)
		{
			map.id = ID;
			map.path = "resources/error.jpg";
			map.type = "texture";
			map.texture = IMG_Load(map.path.c_str());
			if (!map.texture)
				std::cout << "failed to load texture" << SDL_GetError() << "\n";
			else
				std::cout << "loaded texture successfully" << "\n";

			maps.push_back(map);
		}*/
		auto obj = Object3D(std::make_shared<Mesh3D>(fromAssimpMesh(scene->mMeshes[i], maps)));
		meshes.push_back(obj);
	}
	for (int i = 1; i < meshes.size(); i++)
	{
		meshes[0].addChild(meshes[i]);
	}
	return meshes[0];
}

std::vector<Map> loadLightingMaps(aiMaterial* mat, aiTextureType type, std::string typeName, const std::string path)
{
	std::vector<Map> maps;
	for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->Get(AI_MATKEY_NAME, str);
		mat->GetTexture(type, i, &str);
		Map map;
		uint32_t ID;
		glGenTextures(1, &ID);

		// Locate and load the texture image into RAM.
		std::filesystem::path modelPath = path;
		std::filesystem::path texPath = modelPath.parent_path() / str.C_Str();

		map.id = ID;
		map.path = texPath.string();
		map.type = typeName;
		map.texture = IMG_Load(texPath.string().c_str());
		maps.push_back(map);
	}
	return maps;
}
