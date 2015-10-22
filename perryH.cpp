/*
	CS 335 Software Engineering Project

	Group Members:
	Abdulelah Aldeshash
	Eric Smith
	Erik Juarez
	Perry Huynh
	Vincente Lara
*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "ppm.h"
#include "log.h"
/*extern "C" {
	#include "fonts.h"
}*/

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;


//-----------------------------------------------------------------------------
//Setup timers
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
//-----------------------------------------------------------------------------

int xres=1280, yres=960;

struct Ship
{
	Vec dir;
	Vec pos;
	Vec vel;
	float angle;
	Flt radius;
	float color[3];
	int savex;
	int savey;
	Ship()
	{
		VecZero(dir);
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
		pos[2] = 0.0f;
		VecZero(vel);
		angle = 0.0;
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
		savex = 0;
		savey = 0;
	}
};

struct Asteroid
{
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[8];
	float angle;
	float rotate;
	float color[3];
	struct Asteroid *prev;
	struct Asteroid *next;
	Asteroid()
	{
		prev = NULL;
		next = NULL;
	}
};

struct Game
{
	Ship ship;
	Asteroid *ahead;
	int nasteroids;
	struct timespec bulletTimer;
	Game()
	{
		ahead = NULL;
		nasteroids = 0;
	}
};


int keys[65536];

//function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void checkMouse(XEvent *e, Game *g);
int checkKeys(XEvent *e);
void init(Game *g);
void init_sounds(void);
void physics(Game *game);
void render(Game *game);

int main(void)
{
	logOpen();
	initXWindows();
	init_opengl();
	Game game;
	init(&game);
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int done=0;
	while (!done)
	{
		while (XPending(dpy))
		{
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			checkMouse(&e, &game);
			done = checkKeys(&e);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate)
		{
			physics(&game);
			physicsCountdown -= physicsRate;
		}
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	//cleanup_fonts();
	logClose();
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

void init_opengl(void)
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
	//glEnable(GL_TEXTURE_2D);
	//initialize_fonts();
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

void init(Game *g)
{
	g->ship.radius = 40.0;
	//build 10 asteroids...
	for (int j=0; j<30; j++)
	{
		Asteroid *a = new Asteroid;
		a->nverts = 8;
		a->radius = ( rnd() * 2.0 * g->ship.radius ) - ( rnd() * 0.8 * g->ship.radius  );
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
    if (a->radius < g->ship.radius)
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
		//std::cout << "asteroid" << std::endl;
		//add to front of linked list
		a->next = g->ahead;
		if (g->ahead != NULL)
			g->ahead->prev = a;
		g->ahead = a;
		g->nasteroids++;
	}
	memset(keys, 0, 65536);
}

void normalize(Vec v)
{
	Flt len = v[0]*v[0] + v[1]*v[1];
	if (len == 0.0f)
	{
		v[0] = 1.0;
		v[1] = 0.0;
		return;
	}
	len = 1.0f / sqrt(len);
	v[0] *= len * 3;
	v[1] *= len * 3;
}

void checkMouse(XEvent *e, Game *g)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	//
	if (e->type == ButtonRelease)
	{
		return;
	}
	if (e->type == ButtonPress)
	{
		if (e->xbutton.button==1)
		{
			//Left button is down
		}
		if (e->xbutton.button==3)
		{
			//Right button is down
		}
	}
	if (g->ship.savex != e->xbutton.x || g->ship.savey != e->xbutton.y)
	{
		//Mouse moved
		g->ship.savex = e->xbutton.x;
		g->ship.savey = e->xbutton.y;
		int y = yres - e->xbutton.y;
		float dx = g->ship.savex - g->ship.pos[0];
		float dy = y - g->ship.pos[1];
		float len = sqrt(dx * dx + dy * dy);
		g->ship.vel[0] = dx / len;
		g->ship.vel[1] = dy / len;
		normalize(g->ship.vel);
		return;
	}
}

int checkKeys(XEvent *e)
{
	//keyboard input?
	static int shift=0;
	int key = XLookupKeysym(&e->xkey, 0);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease)
	{
		keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift=0;
		return 0;
	}
	if (e->type == KeyPress)
	{
		keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R)
		{
			shift=1;
			return 0;
		}
	}
	else
	{
		return 0;
	}
	if (shift){}
	switch(key)
	{
		case XK_Escape:
			return 1;
		case XK_f:
			break;
		case XK_s:
			break;
		case XK_Down:
			break;
		case XK_equal:
			break;
		case XK_minus:
			break;
	}
	return 0;
}

void deleteAsteroid(Game *g, Asteroid *node)
{
	//remove a node from linked list
	if (g)
	{
		if (node)
		{
			if (node->prev == NULL)
			{
				if (node->next == NULL)
				{
					g->ahead = NULL;
				}
				else
				{
					node->next->prev = NULL;
					g->ahead = node->next;
				}
			}
			else
			{
				if (node->next == NULL)
				{
					node->prev->next = NULL;
				}
				else
				{
					node->prev->next = node->next;
					node->next->prev = node->prev;
				}
			}
			delete node;
			node = NULL;
		}
	}
}

void physics(Game *g)
{
	Flt d0,d1,dist;
	//Update ship position
	g->ship.pos[0] += g->ship.vel[0];
	g->ship.pos[1] += g->ship.vel[1];
	//Check for collision with window edges
	if (g->ship.pos[0] < 0.0)
	{
		g->ship.pos[0] += (float)xres;
	}
	else if (g->ship.pos[0] > (float)xres)
	{
		g->ship.pos[0] -= (float)xres;
	}
	else if (g->ship.pos[1] < 0.0)
	{
		g->ship.pos[1] += (float)yres;
	}
	else if (g->ship.pos[1] > (float)yres)
	{
		g->ship.pos[1] -= (float)yres;
	}

	//Update asteroid positions
	Asteroid *a = g->ahead;
	while (a)
	{
		a->pos[0] += a->vel[0];
		a->pos[1] += a->vel[1];
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

	a = g->ahead;
	while (a)
	{
		//is there a bullet within its radius?
		d0 = g->ship.pos[0] - a->pos[0];
		d1 = g->ship.pos[1] - a->pos[1];
		dist = sqrt(d0*d0 + d1*d1);
		if (dist < (a->radius + g->ship.radius))
		{
			Asteroid *savea = a->next;
			deleteAsteroid(g, a);
			a = savea;
			g->nasteroids--;
			g->ship.radius++;
			if (a == NULL)
				break;
			continue;
		}
		if (a == NULL)
			break;
		a = a->next;
	}
}

void render(Game *g)
{
	glClear(GL_COLOR_BUFFER_BIT);
	//-------------------------------------------------------------------------
	//Draw the ship
	glColor3fv(g->ship.color);
	glPushMatrix();
	glTranslatef(g->ship.pos[0], g->ship.pos[1], g->ship.pos[2]);
	glRotatef(g->ship.angle, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLE_FAN);
	float x = (float)g->ship.radius * cos(499 * PI / 180.f);
	float y = (float)g->ship.radius * sin(499 * PI / 180.f);
	for (int i = 0; i <= 500; i++)
	{
		glVertex2f(x, y);
		x = (float)g->ship.radius * cos(i * PI / 180.f);
		y = (float)g->ship.radius * sin(i * PI / 180.f);
	}
	glEnd();
	glPopMatrix();

	//Draw the asteroids
	Asteroid *a = g->ahead;
	while (a)
	{
		glColor3fv(a->color);
		glPushMatrix();
		glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
		glBegin(GL_TRIANGLE_FAN);
		float x = (float)a->radius * cos(999 * PI / 180.f);
		float y = (float)a->radius * sin(999 * PI / 180.f);
		for (int i = 0; i <= 1000; i++)
		{
			glVertex2f(x, y);
			x = (float)a->radius * cos(i * PI / 180.f);
			y = (float)a->radius * sin(i * PI / 180.f);
		}
		glEnd();
		glPopMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex2f(a->pos[0], a->pos[1]);
		glEnd();
		a = a->next;
	}
}
