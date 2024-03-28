#pragma once
#include <memory>
#include <SFML/Graphics/Transform.hpp>
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
	
	// Transformations.
	void move(const glm::vec3& offset);
	void rotate(const glm::vec3& rotation);
	void grow(const glm::vec3& growth);

	// Rendering.
	void render(sf::RenderWindow& window, Shader& shader) const;

	void addTex(sf::Image texture);
	void cycleTex();
};