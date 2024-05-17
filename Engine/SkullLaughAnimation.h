#pragma once

#include "Object3D.h"
#include "Animation.h"
class SkullLaughAnimation : public Animation {
private:
	glm::vec3 m_translation;
	glm::vec3 m_laughMovement;
	std::vector<Object3D*> m_children;

	void applyAnimation(float_t dt) override {
		object().move(m_translation * dt);
		m_children[0]->move(m_laughMovement * dt);
		m_children[1]->move(m_laughMovement * dt);
	}
public:
	SkullLaughAnimation(Object3D& object, std::vector<Object3D*> children, float_t duration, const glm::vec3& totalMovement) :
		Animation(object, duration), m_children(children), m_translation(totalMovement / duration), m_laughMovement(glm::vec3(0, -1, 0) / duration) {}
};
