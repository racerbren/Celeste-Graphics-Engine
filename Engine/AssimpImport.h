#pragma once
#include "Mesh3D.h"
#include "Object3D.h"
#include <assimp/scene.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Mesh3D fromAssimpMesh(const aiMesh* mesh, const std::vector<Map> maps);
Object3D assimpLoad(const std::string& path, bool flipTextureCoords, bool genNormals, bool genUV);
