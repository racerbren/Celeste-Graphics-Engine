#include "Billboard.h"

Billboard::Billboard(std::shared_ptr<BillboardMesh>&& mesh, glm::vec3 position)
{
	m_position = position;
	m_model = glm::mat4(1.0f);
	m_mesh = mesh;
}

void Billboard::update(glm::vec3 cameraPosition)
{
	//Find the linear distance in 2D between the camera and the billboard
	glm::vec3 distance3D = m_position - cameraPosition;
	float theta{ glm::atan(distance3D.y, distance3D.x) };
	float distance2D{ glm::sqrt(distance3D.x * distance3D.x + distance3D.y * distance3D.y) };
	float phi{ glm::atan(distance3D.z, distance2D) };

	//Update the billboard's model matrix
	m_model = glm::mat4(1.0f);
	m_model = glm::translate(m_model, m_position);
	m_model = m_model * glm::eulerAngleXYZ(0.0f, phi, theta);
}

void Billboard::render(Shader& shader)
{
	shader.setUniform("model", m_model);
	m_mesh->render(shader);
}