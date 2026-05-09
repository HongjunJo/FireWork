// the main routine makes the window, and then runs an even loop
// until the window is closed
#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <ctime>

#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <Fl/Fl_Value_Slider.H>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Light_Button.h>
#include <Fl/Fl_Choice.h>
#include <FL/Fl_Box.h>

#include "MyGlWindow.h"


Fl_Group* widgets;
Fl_Light_Button* test;
Fl_Button* test2;
Fl_Button* test4;

long lastRedraw;
int frameRate = 60;

void changeFrameCB(Fl_Widget * w, void* data)
{
	Fl_Choice * widget = (Fl_Choice *)w;
	int i = widget->value();
	const char * menu = widget->text(i);
	frameRate = atoi(menu);

	MyGlWindow * win = (MyGlWindow *)data;
	win->redraw();
}


void idleCB(void* w)
{
	MyGlWindow * win = (MyGlWindow *)w;
	if (clock() - lastRedraw > CLOCKS_PER_SEC / frameRate) {
		lastRedraw = clock();
		win->update();
	}
	win->redraw();
}


void but_cb(Fl_Widget* o, void*data)
{
	Fl_Button* b = (Fl_Button*)o; //캐스팅이 반드시 필요
	MyGlWindow * win = (MyGlWindow *)data;
	if (b->value())
		win->run = 1;
	else
		win->run = 0;
	win->damage(1);
}


void but_cb2(Fl_Widget* o, void*data)
{
	Fl_Button* b = (Fl_Button*)o; //캐스팅이 반드시 필요
	MyGlWindow * win = (MyGlWindow *)data;
	win->test();
	win->damage(1);
}

void but_cb4(Fl_Widget* o, void* data)
{
	Fl_Button* b = (Fl_Button*)o;
	MyGlWindow* win = (MyGlWindow*)data;
	test->value(0); // run button 을 먼저 off
	win->run = 0; //run 변수를 0으로 변경
	win->step(); //call step()
}

int main()
{
	Fl::scheme("plastic");// plastic
	int width = 800;
	int height = 600;
	Fl_Double_Window* wind = new Fl_Double_Window(100, 100, width, height, "GL 3D FrameWork");

	wind->begin();		// put widgets inside of the window

	widgets = new Fl_Group(0, 0, width, height);
	widgets->begin();

	MyGlWindow* gl = new MyGlWindow(10, 10, width - 20, height - 50);
	Fl::add_idle((void(*)(void*)) idleCB, gl);  //

	widgets->end();
	Fl_Group::current()->resizable(widgets);

	Fl_Choice* choice;
	choice = new Fl_Choice(100, height - 40, 50, 20, "FrameRate");
	choice->add("15");
	choice->add("30");
	choice->add("60");
	choice->value(2);
	choice->callback((Fl_Callback*)changeFrameCB, gl);

	test = new Fl_Light_Button(width - 600, height - 40, 100, 20, "Run");
	test->callback(but_cb, gl);

	test2 = new Fl_Button(width - 400, height - 40, 100, 20, "Test");
	test2->callback(but_cb2, gl);

	test4 = new Fl_Button(width - 200, height - 40, 100, 20, "Step");
	test4->callback(but_cb4, gl);

	gl->ui = test;

	wind->end();

	wind->show();	// this actually opens the window

	Fl::run();
	delete wind;

	return 1;
}

