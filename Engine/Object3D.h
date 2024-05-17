#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Shader.h"
#include "Mesh3D.h"

class Object3D {
private:
	// The object's mesh.
	std::shared_ptr<Mesh3D> m_mesh;

	// The object's position, orientation, and scale in world space.
	glm::vec3 m_position;
	glm::vec3 m_orientation;
	glm::vec3 m_scale;
	glm::vec3 m_center;

	// The object's cached local->world transformation matrix.
	glm::mat4 m_modelMatrix;
	glm::mat4 m_baseTransform;

	// Recomputes the local->world transformation matrix.
	void rebuildModelMatrix();

	//Other meshes in the object if any
	std::vector<Object3D> m_children;

	//This object's material
	glm::vec4 m_material = glm::vec4(0, 0, 0, 0);

public:
	// No default constructor; you must have a mesh to initialize an object.
	Object3D() = delete;

	Object3D(std::shared_ptr<Mesh3D> &&mesh, const glm::mat4 baseTransform);

	int numChildren;

	// Simple accessors.
	const glm::vec3& getPosition() const;
	const glm::vec3& getOrientation() const;
	const glm::vec3& getScale() const;
	const glm::vec3& getCenter() const;

	// Simple mutators.
	void setPosition(const glm::vec3& position);
	void setOrientation(const glm::vec3& orientation);
	void setScale(const glm::vec3& scale);
	void setCenter(const glm::vec3& center);
	void setMaterial(const glm::vec4& material);
	
	// Transformations.
	void move(const glm::vec3& offset);
	void rotate(const glm::vec3& rotation);
	void grow(const glm::vec3& growth);

	void addChild(Object3D&& child);
	const Object3D& getChild(int index) const;
	Object3D& getChild(int index);

	// Rendering.
	void render(Shader& shader, uint32_t shadowMapID) const;
	void renderRecursive(Shader& shader, const glm::mat4& parentMatrix, uint32_t shadowMapID) const;

	void addTex(std::string path, std::string name);
	void cycleTex();
};
