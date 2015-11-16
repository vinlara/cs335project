/*
	CS 335 Software Engineering Project

	Group Members:
	Eric Smith
	Erik Juarez
	Perry Huynh
	Vincente Lara
*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "ppm.h"
#include "structs.h"
#include "perryH.h"
extern "C" {
	#include "fonts.h"
}
using namespace std;

int xres = 1600;
int yres = 900;
int keys[65536];

const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
//unsigned int upause=0;
double timeDiff(struct timespec *start, struct timespec *end)
{
	return (double)(end->tv_sec - start->tv_sec ) +
			(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source)

{
	memcpy(dest, source, sizeof(struct timespec));
}
Display *dpy;
Window win;
GLXContext glc;
Game g;

void initXWindows(void);
void initOpenGL(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void init();
void physics();
void updateCamera();
void setup_screen_res(const int w, const int h);
void set_title(void);
extern void addAsteroid();
extern void deleteAsteroid(Asteroid *node);
extern void asteroidRadiusSpeed(Asteroid *a);

int main(void)
{
	initXWindows();
	initOpenGL();
	loadTempFiles();
	initTextures();
	init();
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	while (!g.done)
	{
		while (XPending(dpy))
		{
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			checkMouse(&e);
			g.done = checkKeys(&e);
		}
		if (g.startScreen)
		{
			if (g.helpScreen)
			{
				renderHelpScreen();
			}
			else
			{
				renderStartScreen();
			}
		}
		else if (g.gameOver)
		{
			renderGameOver();
		}
		else
		{
			clock_gettime(CLOCK_REALTIME, &timeCurrent);
			timeSpan = timeDiff(&timeStart, &timeCurrent);
			timeCopy(&timeStart, &timeCurrent);
			physicsCountdown += timeSpan;
			while (physicsCountdown >= physicsRate)
			{
				physics();
				physicsCountdown -= physicsRate;
			}
			updateCamera();
			render();
		}
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	cleanup_fonts();
	cleanupTempFiles();
	return 0;
}

void cleanupXWindows(void)
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Super Dank Space Adventure");
}

void setup_screen_res(const int w, const int h)
{
	xres = w;
	yres = h;
}

void initXWindows(void)
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
	XSetWindowAttributes swa;
	setup_screen_res(xres, yres);
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	{
		std::cout << "\n\tcannot connect to X server" << std::endl;
		exit(EXIT_FAILURE);
	}

	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL)
	{
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	}

	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask | PointerMotionMask;
	win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void reshape_window(int width, int height)
{
	//window has been resized.
	setup_screen_res(width, height);
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, xres, 0, yres, -1, 1);
	set_title();
}

void initOpenGL(void)
{
	//OpenGL initialization
	glViewport(0, 0, xres, yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}

void check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != xres || xce.height != yres)
	{
		//Window size did change.
		reshape_window(xce.width, xce.height);
	}
}

void init()
{
	//g.score = g.ship.radius;

	cout << g.score << " = g.score (start)\n";

	//build 30 asteroids...
	for (int j=0; j<30; j++)
	{
		Asteroid *a = new Asteroid;
		a->nverts = 8;
		a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.8 * g.ship.radius  );
		a->textureId = rand() % 6;
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++)
		{
			a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
			a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
			angle += inc;
		}
		a->pos[0] = (Flt)(rand() % xres);
		a->pos[1] = (Flt)(rand() % yres);
		a->pos[2] = 0.0f;
		a->angle = 0.0;
		a->rotate = rnd() * 4.0 - 2.0;

		//protect area near center from bigger asteroids
		Flt tmpx = xres >> 2;
		Flt tmpy = yres >> 2;
		if ( a->pos[0] <= (3.0 * tmpx) &&
			a->pos[0] >= (tmpx) &&
			a->pos[1] <= (3.0 * tmpy) &&
			a->pos[1] >= (tmpy)
		   )
		{
		    if (a->pos[0] <= (2.0 * tmpx) &&
			    a->pos[0] > tmpx
		       )//left half
		    {
			a->pos[0] -= 1.25 * tmpx;
			a->pos[2] = 0.0f;
		    }

		    else //right half
		    {
			a->pos[0] += 1.25 * tmpx;
			a->pos[2] = 0.0f;
		    }
		}

		if (a->radius < g.ship.radius)
    		{
    			a->color[0] = 0.9;
    			a->color[1] = 0.6;
    			a->color[2] = 0.3;
    		}

    		else
    		{
			a->color[0] = 0.3;
        		a->color[1] = 0.4;
       			a->color[2] = 0.5;
    		}

    		a->vel[0] = (Flt)(rnd()*2.0-1.0);
		a->vel[1] = (Flt)(rnd()*2.0-1.0);

		asteroidRadiusSpeed(a);

		//add to front of linked list
		a->next = g.ahead;
		if (g.ahead != NULL)
			g.ahead->prev = a;

		g.ahead = a;
		g.nasteroids++;
	}

	memset(keys, 0, 65536);
}

void physics()
{
	Flt d0,d1,dist;
	//Update ship position
	//g.ship.pos[0] += g.ship.vel[0];
	//g.ship.pos[1] += g.ship.vel[1];
	//Check for collision with window edges
	if (g.ship.pos[0] < 0.0)
	{
		g.ship.pos[0] += (float)xres;
	}
	else if (g.ship.pos[0] > (float)xres)
	{
		g.ship.pos[0] -= (float)xres;
	}
	else if (g.ship.pos[1] < 0.0)
	{
		g.ship.pos[1] += (float)yres;
	}
	else if (g.ship.pos[1] > (float)yres)
       	{
		g.ship.pos[1] -= (float)yres;
	}

	//Update asteroid positions
	Asteroid *a = g.ahead;
	while (a)
	{
		a->pos[0] += a->vel[0]; //- g.ship.vel[0];
		a->pos[1] += a->vel[1]; //- g.ship.vel[1];

		//Check for collision with window edges

		if (a->pos[0] < -100.0)
		{
			a->pos[0] += (float)xres+200;
		}

		else if (a->pos[0] > (float)xres+100)
		{
			a->pos[0] -= (float)xres+200;
		}

		else if (a->pos[1] < -100.0)
		{
			a->pos[1] += (float)yres+200;
		}

		else if (a->pos[1] > (float)yres+100)
		{
			a->pos[1] -= (float)yres+200;
		}

		a->angle += a->rotate;
		a = a->next;
	}

	a = g.ahead;
	while (a)
	{
		//is there a bullet within its radius?
		d0 = g.ship.pos[0] - a->pos[0];
		d1 = g.ship.pos[1] - a->pos[1];
		dist = sqrt(d0*d0 + d1*d1);
		if (dist < (a->radius + g.ship.radius))
		{
			if (g.ship.radius >= a->radius)
			{
				Asteroid *savea = a->next;

				//cout << g.score << " g.score (before add)\n"
				//   	<< g.ship.radius << " ship radius\n"
				//	<< a->radius << " asteroid radius\n";

				if (a->radius > 0)
					g.score += 0.5 * a->radius;

				//cout << g.score << " g.score (after add)\n"
				//   	<< g.ship.radius << " ship radius\n\n";

				deleteAsteroid(a);
				a = savea;
				//g.nasteroids--;
				g.ship.radius += 0.5 * log2(g.score);
				addAsteroid();
				if (a == NULL)
					break;
				continue;
			}
			else
			{
				g.gameOver = 1;
			}
		}
		if (a == NULL)
			break;
		a = a->next;
	}
}

void updateCamera()
{
	//cout << (float)xres << " xres\n";
	//cout << (float)yres << " yres\n";
	//cout << std::dec << (g.ship.pos) << " g.ship.pos\n";
	bool Move = false;
	if(g.ship.pos[0] < (float)(xres)*.65+.1 && g.ship.pos[0] > (float)(xres)*.35-.1
		&& g.ship.pos[1] < (float)(yres)*.65+.1 && g.ship.pos[1] > (float)(yres)*.35-.1)
		{
			//Update ship position
			g.ship.pos[0] += g.ship.vel[0];
			g.ship.pos[1] += g.ship.vel[1];
			Move = false;
		}
		else
		{
			Move = true;
			if(g.ship.pos[0] > (float)(xres)*.65+.1)
				g.ship.pos[0] = (float)(xres)*.65;
			if(g.ship.pos[0] < (float) (xres)*.35-.1)
				g.ship.pos[0] = (float)(xres)*.35;
			if(g.ship.pos[1] > (float)(yres)*.65+.1)
				g.ship.pos[1] = (float)(yres)*.65;
			if(g.ship.pos[1] < (float)(yres)*.35-.1)
				g.ship.pos[1] = (float)(yres)*.35;
		}

	Asteroid *a = g.ahead;
	if (Move)
	{
		while (a)
		{
			a->pos[0] -= g.ship.vel[0] * 2;
			a->pos[1] -= g.ship.vel[1] * 2;
			a = a->next;
		}
	}
}
