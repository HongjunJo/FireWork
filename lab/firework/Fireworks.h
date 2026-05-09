#pragma once
#include <vector>
#include "Fire.h"
#include "FireworksRule.h"

class Fireworks
{
public:
    Fireworks();
    ~Fireworks();

    void update(float duration);
    void draw(int shadow);
    void create();

    std::vector<Fire*> fireworks;
    FireworksRule m_rule[5];

	float m_elapsedTime;    // 시퀀스 타이밍을 위한 누적 시간
	int m_sequenceStep;     // 시퀀스 단계 추적
	bool m_isPlaying;       // 연출이 진행 중인지 여부

    void LaunchInitFire(cyclone::Vector3 pos, cyclone::Vector3 vel, cyclone::Vector3 color, float age, int ruleIndex);
};