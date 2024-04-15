#include <SDL2/SDL.h>
#include <glm/ext.hpp>

#include "Object3D.h"
#include "Shader.h"

void Object3D::rebuildModelMatrix() 
{
	auto m = glm::translate(glm::mat4(1), m_position);
	m = glm::scale(m, m_scale);
	m = glm::rotate(m, m_orientation[2], glm::vec3(0, 0, 1));
	m = glm::rotate(m, m_orientation[0], glm::vec3(1, 0, 0));
	m = glm::rotate(m, m_orientation[1], glm::vec3(0, 1, 0));
	m_modelMatrix = m;
}

Object3D::Object3D(std::shared_ptr<Mesh3D>&& mesh) : m_mesh(mesh), m_position(), m_orientation(), m_scale(1.0) 
{
	rebuildModelMatrix();
}

const glm::vec3& Object3D::getPosition() const 
{
	return m_position;
}

const glm::vec3& Object3D::getOrientation() const 
{
	return m_orientation;
}

const glm::vec3& Object3D::getScale() const 
{
	return m_scale;
}

void Object3D::setPosition(const glm::vec3& position) 
{
	m_position = position;
	rebuildModelMatrix();
}

void Object3D::setOrientation(const glm::vec3& orientation) 
{
	m_orientation = orientation;
	rebuildModelMatrix();
}

void Object3D::setScale(const glm::vec3& scale) 
{
	m_scale = scale;
	rebuildModelMatrix();
}

void Object3D::setMaterial(const glm::vec4& material)
{
	m_material = material;
	for (auto& child : m_children)
		child.setMaterial(material);
}

void Object3D::move(const glm::vec3& offset) 
{
	m_position = m_position + offset;
	rebuildModelMatrix();
}

void Object3D::rotate(const glm::vec3& rotation) 
{
	m_orientation = m_orientation + rotation;
	rebuildModelMatrix();
}

void Object3D::grow(const glm::vec3& growth) 
{
	m_scale = m_scale * growth;
	rebuildModelMatrix();
}

void Object3D::addChild(Object3D child)
{
	m_children.push_back(child);
}

void Object3D::render(Shader& shader) const 
{
	renderRecursive(shader, glm::mat4(1));
}

void Object3D::renderRecursive(Shader& shader, const glm::mat4& parentMatrix) const
{
	glm::mat4 trueModel = parentMatrix * m_modelMatrix;
	shader.setUniform("model", trueModel);
	shader.setUniform("material", m_material);
	m_mesh->render(shader);

	for (auto& child : m_children) {
		child.renderRecursive(shader, trueModel);
	}
}
