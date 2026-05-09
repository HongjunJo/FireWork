#include "Fireworks.h"
#include "random.h"

Fireworks::Fireworks() {
    m_elapsedTime = 0.0f;
    m_sequenceStep = 0;
    m_isPlaying = false;

    m_rule[0].setParameters(SHAPE_SPHERE, 20.0f, 0.9f, 40);
    m_rule[1].setParameters(SHAPE_SPHERE, 35.0f, 0.85f, 100);
    m_rule[2].setParameters(SHAPE_SPHERE, 40.0f, 0.98f, 150);
    m_rule[3].setParameters(SHAPE_RANDOM, 25.0f, 0.95f, 60);
}

Fireworks::~Fireworks() {
    for (Fire* f : fireworks) { delete f; }
    fireworks.clear();
}

void Fireworks::LaunchInitFire(cyclone::Vector3 pos, cyclone::Vector3 vel, cyclone::Vector3 color, float age, int ruleIndex) {
    Fire* initFire = new Fire(0);
    initFire->m_particle->setPosition(pos.x, pos.y, pos.z);
    initFire->m_particle->setVelocity(vel.x, vel.y, vel.z);
    initFire->setColor(color.x, color.y, color.z);
    initFire->setAge(age);
    initFire->setRule(&m_rule[ruleIndex]);
    fireworks.push_back(initFire);
}

void Fireworks::create() {
    // 기존 불꽃 파티클을 모두 제거하고 상태를 초기화
    for (Fire* f : fireworks) { delete f; }
    fireworks.clear();

    m_elapsedTime = 0.0f;
    m_sequenceStep = 0;
    m_isPlaying = true;
}

void Fireworks::update(float duration) {
    if (m_isPlaying) {
        m_elapsedTime += duration;

        // [시퀀스 0] 0.5초: 다채로운 불꽃 발사
        if (m_sequenceStep == 0 && m_elapsedTime >= 0.5f) {
            float startX = -40.0f;
            float stepX = 80.0f / 4.0f;

            // 5가지의 축제 느낌 색상 배열 정의
            cyclone::Vector3 festivalColors[5] = {
                cyclone::Vector3(1.0f, 0.2f, 0.2f),
                cyclone::Vector3(0.2f, 1.0f, 0.2f),
                cyclone::Vector3(0.2f, 0.5f, 1.0f),
                cyclone::Vector3(1.0f, 0.2f, 1.0f),
                cyclone::Vector3(0.2f, 1.0f, 1.0f)
            };

            for (int i = 0; i < 5; i++) {
                float currentX = startX + (i * stepX);
                float currentZ = 15.0f + abs(currentX) * 0.5f;
                float currentVy = 35.0f + (i % 2) * 5.0f;
                float currentAge = 1.0f + (i % 2) * 0.2f;

                LaunchInitFire(cyclone::Vector3(currentX, 0, currentZ), cyclone::Vector3(0, currentVy, 0), festivalColors[i], currentAge, 0);
            }
            m_sequenceStep++;
        }

        // [시퀀스 1] 2.5초: 대나무 느낌의 불꽃 발사
        else if (m_sequenceStep == 1 && m_elapsedTime >= 2.5f) {
            LaunchInitFire(cyclone::Vector3(0, 0, 10), cyclone::Vector3(-30, 38, 0), cyclone::Vector3(1.0f, 0.3f, 0.8f), 1.3f, 0);
            LaunchInitFire(cyclone::Vector3(0, 0, 10), cyclone::Vector3(30, 38, 0), cyclone::Vector3(1.0f, 0.3f, 0.8f), 1.3f, 0);
            LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(-15, 42, 0), cyclone::Vector3(0.2f, 0.8f, 1.0f), 1.4f, 0);
            LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(15, 42, 0), cyclone::Vector3(0.2f, 0.8f, 1.0f), 1.4f, 0);
            LaunchInitFire(cyclone::Vector3(0, 0, -10), cyclone::Vector3(0, 45, 0), cyclone::Vector3(1.0f, 1.0f, 0.2f), 1.5f, 0);
            m_sequenceStep++;
        }

        // [시퀀스 2] 4.5초: 아치형 불꽃 발사
        else if (m_sequenceStep == 2 && m_elapsedTime >= 4.5f) {
            // 중앙 발사
            LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(0, 48, 0), cyclone::Vector3(1.0f, 1.0f, 0.3f), 2.2f, 1);

            // 안쪽 좌우 대칭 발사
            LaunchInitFire(cyclone::Vector3(-12, 0, 0), cyclone::Vector3(-8, 44, 0), cyclone::Vector3(1.0f, 0.2f, 0.6f), 2.0f, 1);
            LaunchInitFire(cyclone::Vector3(12, 0, 0), cyclone::Vector3(8, 44, 0), cyclone::Vector3(1.0f, 0.2f, 0.6f), 2.0f, 1);

            // 바깥쪽 좌우 대칭 발사
            LaunchInitFire(cyclone::Vector3(-24, 0, 0), cyclone::Vector3(-16, 40, 0), cyclone::Vector3(0.2f, 0.6f, 1.0f), 1.8f, 1);
            LaunchInitFire(cyclone::Vector3(24, 0, 0), cyclone::Vector3(16, 40, 0), cyclone::Vector3(0.2f, 0.6f, 1.0f), 1.8f, 1);
            m_sequenceStep++;
        }

        // [시퀀스 3] 7.5초: 대칭 형태의 포물선 난사 발사 (파도타기 연출)
        else if (m_sequenceStep == 3 && m_elapsedTime >= 7.5f) {
            static cyclone::Random crandom;

            // 중앙 기둥 발사: 정확한 각도를 기반으로 5발의 불꽃을 균일하게 배치
            for (int i = -2; i <= 2; i++) {
                float vx = i * 4.0f;
                float vy = 55.0f - abs(i) * 2.0f;
                float vz = crandom.randomReal(-8.0f, 8.0f);
                float randAge = 1.6f + crandom.randomReal(-0.1f, 0.1f);

                cyclone::Vector3 centerColor = cyclone::Vector3(1.0f, 0.9f, 0.7f);
                LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(vx, vy, vz), centerColor, randAge, 3);
            }

            // 좌우 대칭 발사: 10쌍의 불꽃을 선형 보간(Lerp) 적용하여 발사
            int pairCount = 10;
            for (int i = 0; i < pairCount; i++) {
                float t = (float)i / (pairCount - 1);

                float vx = 12.0f + (t * 28.0f);
                float vy = 50.0f - (t * 15.0f);
                float vz = crandom.randomReal(-25.0f, 25.0f);

                float waveAge = 1.2f + (t * 1.0f);
                cyclone::Vector3 randColor = crandom.randomVector(cyclone::Vector3(0.4f, 0.2f, 0.2f), cyclone::Vector3(1.0f, 1.0f, 1.0f));

                LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(vx, vy, vz), randColor, waveAge, 3);
                LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(-vx, vy, vz), randColor, waveAge, 3);
            }
            m_sequenceStep++;
        }

        // [시퀀스 4] 10.5초: 피날레 발사 (넓은 영역에서 대형 불꽃 점화)
        else if (m_sequenceStep == 4 && m_elapsedTime >= 10.5f) {
            // 중앙 발사 (가장 높게)
            LaunchInitFire(cyclone::Vector3(0, 0, 0), cyclone::Vector3(0, 58, 0), cyclone::Vector3(1.0f, 0.95f, 0.6f), 2.2f, 2);

            // 중간 좌우 발사
            LaunchInitFire(cyclone::Vector3(-25, 0, 5), cyclone::Vector3(-15, 52, 0), cyclone::Vector3(1.0f, 0.6f, 0.2f), 1.9f, 2);
            LaunchInitFire(cyclone::Vector3(25, 0, 5), cyclone::Vector3(15, 52, 0), cyclone::Vector3(1.0f, 0.6f, 0.2f), 1.9f, 2);

            // 바깥 좌우 발사
            LaunchInitFire(cyclone::Vector3(-50, 0, 15), cyclone::Vector3(-25, 46, -5), cyclone::Vector3(1.0f, 0.8f, 0.1f), 1.6f, 2);
            LaunchInitFire(cyclone::Vector3(50, 0, 15), cyclone::Vector3(25, 46, -5), cyclone::Vector3(1.0f, 0.8f, 0.1f), 1.6f, 2);

            m_sequenceStep++;
        }
    }

    std::vector<Fire*>::iterator iter;
    std::vector<Fire*> remove;

    for (iter = fireworks.begin(); iter != fireworks.end(); ) {
        Fire* m = *iter;

        if (m->update(duration)) {
            if (m->m_type == 0 && m->m_rule != nullptr) {
                remove.push_back(m);
            }
            else {
                delete m;
            }
            iter = fireworks.erase(iter);
        }
        else {
            ++iter;
        }
    }

    for (Fire* parent : remove) {
        parent->m_rule->spawn(parent, fireworks);
        delete parent;
    }
}

void Fireworks::draw(int shadow) {
    for (Fire* f : fireworks) f->draw(shadow);
}