#include "AssimpImport.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>


const size_t FLOATS_PER_VERTEX = 3;
const size_t VERTICES_PER_FACE = 3;

Mesh3D fromAssimpMesh(const aiMesh* mesh, const sf::Image& texture) {
	std::vector<Vertex3D> vertices;

	// TODO: fill in this vertices list, by iterating over each element of 
	// the mVertices field of the aiMesh pointer. Each element of mVertices
	// has x, y, and z values that you can use to construct a Vertex3D object.
	// To find the u and v texture coordinates of a vertex, access the 
	// x and y fields of each element of mTextureCoords.
	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		auto& meshVertex = mesh->mVertices[i];
		auto& texCoord = mesh->mTextureCoords[0][i];
		auto vertex = Vertex3D(meshVertex.x, meshVertex.y, meshVertex.z, texCoord.x, texCoord.y);
		vertices.push_back(vertex);
	}

	std::vector<uint32_t> faces;
	// TODO: fill in the faces list, by iterating over each element of
	// the mFaces field of the aiMesh pointer. Each element of mFaces
	// has an mIndices list, which will have three elements of its own at 
	// [0], [1], and [2]. Each of those should be pushed individually onto 
	// the faces list.
	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		auto& meshFace = mesh->mFaces[i];
		faces.push_back(meshFace.mIndices[0]);
		faces.push_back(meshFace.mIndices[1]);
		faces.push_back(meshFace.mIndices[2]);
	}

	return Mesh3D(vertices, faces, texture);
}

Object3D assimpLoad(const std::string& path, bool flipTextureCoords) {
	Assimp::Importer importer;

	auto options = aiProcessPreset_TargetRealtime_MaxQuality;
	if (flipTextureCoords) {
		options |= aiProcess_FlipUVs;
	}
	const aiScene* scene = importer.ReadFile(path, options);

	// If the import failed, report it
	if (nullptr == scene) {
		throw std::runtime_error("Error loading assimp file ");
		
	}
	else {

	}
	auto* mesh = scene->mMeshes[0];
	sf::Image texture;

	if (scene->HasMaterials()) {
		// Locate the "diffuse map" of the mesh, which is the mesh's primary texture.
		auto* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		material->GetTexture(aiTextureType_DIFFUSE, 0, &name);

		// Locate and load the texture image into RAM.
		std::filesystem::path modelPath = path;
		std::filesystem::path texPath = modelPath.parent_path() / name.C_Str();
		texture.loadFromFile(texPath.string());
	}
	auto ret = Object3D(std::make_shared<Mesh3D>(fromAssimpMesh(scene->mMeshes[0], texture)));
	return ret;
}
