/*  Eric Smith
    ericS.cpp

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
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
							 (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
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

struct Ship {
	Vec dir;
	Vec pos;
	Vec vel;
	float angle;
	Flt radius;
	float color[3];
	Ship() {
		VecZero(dir);
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
		pos[2] = 0.0f;
		VecZero(vel);
		angle = 0.0;
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
	}
};

struct Asteroid {
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
	Asteroid() {
		prev = NULL;
		next = NULL;
	}
};

struct Game {
	Ship ship;
	Asteroid *ahead;
	int nasteroids;
	struct timespec bulletTimer;
	Game() {
		ahead = NULL;
		nasteroids = 0;
	}
};


int keys[65536];

//function prototypes
void init(Game *g);
void render(Game *game);

void init(Game *g) {
	g->ship.radius = 40.0;
	//build 10 asteroids...
	for (int j=0; j<30; j++) {
		Asteroid *a = new Asteroid;
		a->radius = ( rnd() * 2.0 * g->ship.radius ) - ( rnd() * 0.8 * g->ship.radius  );
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++) {
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

void addAsteroid(Game *g) {

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
	while (a) {
        if (a->radius < g->ship.radius) {
    	    a->color[0] = 0.9;
    	    a->color[1] = 0.6;
    	    a->color[2] = 0.3;
        }
        else {
            a->color[0] = 0.3;
            a->color[1] = 0.4;
            a->color[2] = 0.5;
        }
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
