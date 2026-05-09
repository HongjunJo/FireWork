#pragma once
#include <vector>
#include "particle.h"

class Fire;

enum ShapeType {
    SHAPE_CIRCLE,  // 2D 원형
    SHAPE_SPHERE,  // 3D 구형 (입체)
    SHAPE_RANDOM   // 무작위 난사
};

class FireworksRule
{
public:
    FireworksRule() {};
    ~FireworksRule() {};

	ShapeType shape; // 폭발 형태 (원형, 구형, 무작위)
	cyclone::real force; // 폭발력 (초기 속도 크기)
    cyclone::real damping; // 감쇠율
    unsigned payloadCount; // 폭발 시 생성되는 입자 수

    void setParameters(ShapeType shape, cyclone::real force, cyclone::real damping, int count)
    {
        this->shape = shape;
        this->force = force;
        this->damping = damping;
        this->payloadCount = count;
    }

    void spawn(Fire* parent, std::vector<Fire*>& fireworksContainer);
};