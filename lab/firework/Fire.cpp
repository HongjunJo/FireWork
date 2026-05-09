#include "Fire.h"

Fire::Fire(int type)
{
    m_particle = new cyclone::Particle();
    m_type = type;
    m_size = 0.5f;
    m_rule = nullptr;

    m_particle->setMass(1.0f);
    m_particle->setDamping(0.99f);
    m_particle->setAcceleration(0, -9.8f, 0);
    m_color = cyclone::Vector3(1, 1, 1);
    m_age = 1.0f;
}

bool Fire::update(float duration) {
    m_particle->integrate(duration);

    cyclone::Vector3 pos = m_particle->getPosition();
	if (m_history.size() >= 30) {
        m_history.pop_front();
    }
    m_history.push_back(pos);

    m_age -= duration;

    if (m_age < 0.0f || pos.y < 0.0f) return true;
    return false;
}

void Fire::draw(int shadow) {
    cyclone::Vector3 position = m_particle->getPosition();
    if (shadow)
        glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    else
        glColor3f(m_color.x, m_color.y, m_color.z);

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    glutSolidSphere(m_size, 8, 8);

    glPopMatrix();

    if (!shadow) drawHistory();
}

void Fire::setRule(FireworksRule* r) { m_rule = r; }
void Fire::CheckEdges() {} // 폭발 입자는 바닥에 닿으면 사라지도록 처리 (y < 0)

void Fire::drawHistory() {
    glLineWidth(2.0f);
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
    glColor3f(m_color.x, m_color.y, m_color.z);
    for (unsigned int i = 0; i < m_history.size(); i++) {
        cyclone::Vector3 p = m_history[i];
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    glPopMatrix();
    glLineWidth(1.0f);
}