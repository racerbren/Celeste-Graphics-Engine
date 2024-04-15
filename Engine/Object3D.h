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

	// The object's cached local->world transformation matrix.
	glm::mat4 m_modelMatrix;

	// Recomputes the local->world transformation matrix.
	void rebuildModelMatrix();

	//Other meshes in the object if any
	std::vector<Object3D> m_children;

	//This object's material
	glm::vec4 m_material = glm::vec4(0, 0, 0, 0);

public:
	// No default constructor; you must have a mesh to initialize an object.
	Object3D() = delete;

	Object3D(std::shared_ptr<Mesh3D> &&mesh);

	// Simple accessors.
	const glm::vec3& getPosition() const;
	const glm::vec3& getOrientation() const;
	const glm::vec3& getScale() const;

	// Simple mutators.
	void setPosition(const glm::vec3& position);
	void setOrientation(const glm::vec3& orientation);
	void setScale(const glm::vec3& scale);
	void setMaterial(const glm::vec4& material);
	
	// Transformations.
	void move(const glm::vec3& offset);
	void rotate(const glm::vec3& rotation);
	void grow(const glm::vec3& growth);

	void addChild(Object3D child);

	// Rendering.
	void render(Shader& shader) const;
	void renderRecursive(Shader& shader, const glm::mat4& parentMatrix) const;
};
