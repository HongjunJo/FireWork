
#define NOMINMAX


#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <Fl/Fl_Light_Button.h>
#include <Fl/Fl_Value_Slider.H>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <Fl/Fl_Double_Window.h>
#include <vector>

#include "stdio.h"
#include "math.h"
#include "3DUtils.h"

#include "Viewer.h"
#include "Fireworks.h"

#include "core.h"
#include "pfgen.h"


class MyGlWindow : public Fl_Gl_Window {
public:
	MyGlWindow(int x, int y, int w, int h);

	Fl_Light_Button* ui;

	Fl_Slider* time;
	void update();
	void drawStuff();
	void doPick();
	void test();
	void step();

	int selected;
	int run;

	cyclone::ParticleForceRegistry m_forces;
	cyclone::ParticleGravity* m_gravity;
	cyclone::ParticleDrag* m_drag;

private:
	void draw();					// standard FlTk
	int handle(int);				// standard FlTk

	float fieldOfView;
	Viewer *m_viewer;
	Fireworks *m_fireworks;

	void putText(char* string, int x, int y, float r, float g, float b);
	void setProjection(int clearProjection = 1);
	void getMouseNDC(float& x, float& y);
	void setupLight(float x, float y, float z);
};

