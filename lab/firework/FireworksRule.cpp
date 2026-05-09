#include "FireworksRule.h"
#include "Fire.h"
#include "random.h"
#include <math.h>

void FireworksRule::spawn(Fire* parent, std::vector<Fire*>& fireworksContainer) {
    cyclone::Vector3 parentPos = parent->m_particle->getPosition();
    cyclone::Vector3 parentVel = parent->m_particle->getVelocity();
    cyclone::Vector3 parentColor = parent->m_color;

    static cyclone::Random crandom;

    for (unsigned int i = 0; i < payloadCount; i++) {
        Fire* child = new Fire(1);
		child->m_particle->setPosition(parentPos); // 부모의 위치에서 시작
        child->setColor(parentColor.x, parentColor.y, parentColor.z); 
        child->setAge(crandom.randomReal(1.0f, 2.5f));
        child->m_particle->setMass(1.0f);
        child->m_particle->setAcceleration(0, -9.8f, 0);
        child->m_particle->setDamping(this->damping);

		cyclone::Vector3 explosionVel; // 폭발 방향과 속도 계산

		if (this->shape == SHAPE_CIRCLE) { // 원형 폭발 - 2D 평면
            float angle = i * ((2.0f * 3.141592f) / payloadCount);
            explosionVel = cyclone::Vector3(cos(angle) * force, sin(angle) * force, 0.0f);
        }
		else if (this->shape == SHAPE_SPHERE) { // 구형 폭발 - 3D 공간
            // 구면 좌표계를 이용한 3D 구형 폭발
            float phi = acos(1.0f - 2.0f * (float)i / payloadCount); 
            float theta = 3.141592f * (1.0f + sqrt(5.0f)) * (float)i;
            explosionVel = cyclone::Vector3(sin(phi) * cos(theta) * force, sin(phi) * sin(theta) * force, cos(phi) * force); 
        }
        else { // SHAPE_RANDOM
            explosionVel = crandom.randomVector(cyclone::Vector3(-force, -force, -force), cyclone::Vector3(force, force, force));
        }

        // 부모의 관성에 폭발 힘을 더함
        child->m_particle->setVelocity(parentVel + explosionVel);
        fireworksContainer.push_back(child);
    }
}