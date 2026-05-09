

#include "MyGlWindow.h"


#include <iostream>
#include <vector>
#include "drawUtils.h"

#include <time.h>
#include "timing.h"


static double DEFAULT_VIEW_POINT[3] = { 30, 30, 30 };
static double DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static double DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };
static cyclone::Vector3 p1, p2;
static clock_t t1 = 0, t2 = 0;

MyGlWindow::MyGlWindow(int x, int y, int w, int h) :
	Fl_Gl_Window(x, y, w, h)
	//==========================================================================
{

	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);

	fieldOfView = 45;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);

	TimingData::init();
	run = 0;
	selected = -1;

	m_fireworks = new Fireworks();

	m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -9.8f, 0));
	m_drag = new cyclone::ParticleDrag(0.1f, 0.01f);

	m_gravity->enable = true;
	m_drag->enable = true;
}


void MyGlWindow::setupLight(float x, float y, float z)
{

	// set up the lighting
	GLfloat lightPosition[] = { 500, 900.0, 500, 1.0 };
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };

	GLfloat violetLight[] = { 0.5f, 0.1f, .5f, 1.0 };
	GLfloat whiteLight[] = { 1, 1, 1, 1.0 };
	GLfloat whiteLight2[] = { .3, .3, .3, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };


	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	lightPosition[0] = x;
	lightPosition[1] = y;
	lightPosition[2] = z;

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, whiteLight2);
	
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLight);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}




void setupObjects(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0x0, 0x0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilMask(0x1);		// only deal with the 1st bit
}


void MyGlWindow::drawStuff()
{
	glColor4f(1, 1, 0, 0.5);
	polygonf(4, 20., 0., -25., 20., 0., 25., -20., 30., 25., -20., 30., -25.);
}

void drawStrokeText(char* string, int x, int y, int z)
{
	char* c;
	glPushMatrix();
	glTranslated(x, y + 8, z);
	glScaled(0.2, 0.2, 0.2);
	for (c = string; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();
}


void MyGlWindow::putText(char* string, int x, int y, float r, float g, float b)
{

	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ortho();


	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);


	glColor3f(r, g, b);
	drawStrokeText(string, x, y, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);


}

//==========================================================================
void MyGlWindow::draw()
//==========================================================================
{

	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0.2f, 0.2f, .2f, 0);		// background should be blue


	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);



	// now draw the ground plane
	setProjection();
	setupFloor();

	glPushMatrix();
	drawFloor(200, 20);
	glPopMatrix();


	setupLight(m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z);

	//draw shadow
	setupShadows();
	m_fireworks->draw(1);
	unsetupShadows();

	glEnable(GL_LIGHTING);
	//draw objects
	glPushMatrix();
	m_fireworks->draw(0);
	glPopMatrix();

	glDisable(GL_BLEND);
	putText("HongjunJo", 10, 10, 1, 1, 0);

	glViewport(0, 0, w(), h());
	setProjection();
	glEnable(GL_COLOR_MATERIAL);
}

void MyGlWindow::test()
{
	m_fireworks->create();
}

void MyGlWindow::update()
{
	// 프레임 시간 업데이트 
	TimingData::get().update();

	// 시뮬레이션이 실행 중이 아니면 업데이트하지 않음
	if (!run) 
		return;

	// 프레임 시간에 따른 시뮬레이션 업데이트
	float duration = (float)TimingData::get().lastFrameDuration * 0.003;

	// duration이 0보다 작거나 같으면 업데이트하지 않음 (예외 처리)
	if (duration <= 0.0f) return;

	// 적용되는 힘 업데이트
	m_forces.updateForces(duration);

	m_fireworks->update(duration);
}


void MyGlWindow::doPick()
{
	make_current();		// since we'll need to do some GL stuff

	int mx = Fl::event_x(); // where is the mouse?
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my), 5, 5, viewport);

	// now set up the projection
	setProjection(false);

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// No objects to pick natively


	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selected = buf[3] - 1;
	}
	else {// nothing hit, nothing selected
		selected = -1;
	}
	//printf("Selected Cube %d\n", selectedCube);
}



void MyGlWindow::setProjection(int clearProjection)
//==========================================================================
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, w(), h());
	if (clearProjection)
		glLoadIdentity();
	// compute the aspect ratio so we don't distort things
	double aspect = ((double)w()) / ((double)h());
	gluPerspective(fieldOfView, aspect, 1, 1000);

	// put the camera where we want it to be
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// use the transformation in the ArcBall

	gluLookAt(
		m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z,
		m_viewer->getViewCenter().x, m_viewer->getViewCenter().y, m_viewer->getViewCenter().z,
		m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z
	);

//	glDisable(GL_BLEND);
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e)
//==========================================================================
{
	switch (e) {
	case FL_SHOW:		// you must handle this, or not be seen!
		show();
		return 1;
	case FL_MOUSEWHEEL: // 휠 액션 추가 
	{
		float wheelDelta = (float)Fl::event_dy();
		m_viewer->zoom(-wheelDelta * 0.1f); // 유니티 방향과 맞추기 위해 음수 적용
		redraw();
		return 1;
	}
	case FL_PUSH: // 클릭
	{
		m_pressedMouseButton = Fl::event_button();
		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();


		if (m_pressedMouseButton == 1) {
			doPick();
			if (selected >= 0) {
				// 파티클 잡았으니, 시간 변경
				run = 0;
				ui->value(0);

				t1 = clock();

				std::cout << "picked" << std::endl;
			}

			damage(1);
			return 1;
		};
		break;
	}

	damage(1);
	return 1;
	case FL_RELEASE: // 드래그 끝났을 때
		if (selected >= 0 && m_pressedMouseButton == 1) {
			t2 = clock();

			float dt = float(t2 - t1) / CLOCKS_PER_SEC;

			// 드레그 끝났으니, 시간 변경
			run = 1;
			ui->value(1);
			selected = -1;
		}
		m_pressedMouseButton = -1;
		damage(1);
		return 1;

	case FL_DRAG: // 드래그
	{
		if (selected >= 0 && m_pressedMouseButton == 1) {
			damage(1);
		}
		else {


			double fractionChangeX = static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
			double fractionChangeY = static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

			if (m_pressedMouseButton == 3) { // 우클릭 (FL_RIGHT_MOUSE): 유니티처럼 카메라 회전
				m_viewer->rotate(fractionChangeX, fractionChangeY);
			}
			else if (m_pressedMouseButton == 2) { // 휠클릭(가운데 버튼, FL_MIDDLE_MOUSE): 카메라 패닝 (Translate)
				m_viewer->translate(-fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
			}
			else if (m_pressedMouseButton == 1) { 
				// 좌클릭 (FL_LEFT_MOUSE): 원래는 객체 선택 및 드래그용이지만, 임의로 카메라 패닝(이동) 기능으로 할당함
				m_viewer->translate(-fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
			}
			else {
				std::cout << "Warning: dragging with unknown mouse button!  Nothing will be done" << std::endl;
			}

			m_lastMouseX = Fl::event_x();
			m_lastMouseY = Fl::event_y();
			redraw();
		}
	}

	return 1;

	case FL_KEYBOARD:
		return 0;

	default:
		return 0;
	}
}


//
// get the mouse in NDC
//==========================================================================
void MyGlWindow::getMouseNDC(float& x, float& y)
//==========================================================================
{
	// notice, we put everything into doubles so we can do the math
	float mx = (float)Fl::event_x();	// remeber where the mouse went down
	float my = (float)Fl::event_y();

	// we will assume that the viewport is the same as the window size
	float wd = (float)w();
	float hd = (float)h();

	// remember that FlTk has Y going the wrong way!
	my = hd - my;

	x = (mx / wd) * 2.0f - 1.f;
	y = (my / hd) * 2.0f - 1.f;
}

void MyGlWindow::step()
{
	TimingData::get().update();
	float duration = 0.03; // or 0.06

	// 프레임 시간에 따른 시뮬레이션 업데이트
	m_forces.updateForces(duration);

	std::cout << "step" << std::endl;
}