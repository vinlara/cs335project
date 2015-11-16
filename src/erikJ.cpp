// Erik Juarez
// Lab 6

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, 1280, 960);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, 1280, 0, 960, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	//glEnable(GL_TEXTURE_2D);
	//initialize_fonts();
}
