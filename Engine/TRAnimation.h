#pragma once

#include "Object3D.h"
#include "Animation.h"
class TRAnimation : public Animation {
private:
	glm::vec3 m_translation;
	glm::vec3 m_perSecond;

	void applyAnimation(float_t dt) override {
		object().move(m_translation * dt);
		object().rotate(m_perSecond * dt);
	}
public:
	TRAnimation(Object3D& object, float_t duration, const glm::vec3& totalMovement, const glm::vec3& totalRotation) :
		Animation(object, duration), m_translation(totalMovement / duration), m_perSecond(totalRotation / duration) {}
};
