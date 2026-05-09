#ifndef __FIRE_H
#define __FIRE_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <deque>

#include "core.h"
#include "particle.h"

class FireworksRule;

class Fire
{
public:
    cyclone::Particle* m_particle;

    Fire(int type);
    ~Fire() { delete m_particle; };

    float m_size;
    int m_type;
    float m_age;
    cyclone::Vector3 m_color;
    std::deque<cyclone::Vector3> m_history;
    FireworksRule* m_rule;

    bool update(float duration);
    void draw(int shadow);
    void setRule(FireworksRule* r);
    void CheckEdges();
    void drawHistory();

	// setters
    void setColor(float r, float g, float b) { m_color = cyclone::Vector3(r, g, b); }
    void setAge(float age) { m_age = age; }
};
#endif