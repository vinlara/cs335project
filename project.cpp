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
extern "C" {
	#include "fonts.h"
}
using namespace std;
#define USE_SOUND
#ifdef USE_SOUND
#include <FMOD/fmod.h>
#include <FMOD/wincompat.h>
#include "fmod.h"
#endif

Display *dpy;
Window win;
GLXContext glc;
int xres = 1600;
int yres = 900;
int keys[65536];

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

Game g;

void initXWindows(void);
void initOpenGL(void);
extern void initTextures(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void checkMouse(XEvent *e);
int checkKeys(XEvent *e);
void init();
void initSounds(void);
extern void renderStartScreen();
extern void renderGameOver();
void physics();
void updateCamera();
void updateScore();
extern void render();
void normalize(Vec v);
void setup_screen_res(const int w, const int h);
void deleteAsteroid(Asteroid *node);
void set_title(void);
void addAsteroid();

int main(void)
{
	initXWindows();
	initOpenGL();
	initTextures();
	init();
	initSounds();
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
			renderStartScreen();
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
	#ifdef USE_SOUND
	fmod_cleanup();
	#endif //USE_SOUND
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

/*void initTextures(void)
{
	//load the images file into a ppm structure.
	startScreen = ppm6GetImage("images/startScreen.ppm");
	gameOver = ppm6GetImage("images/gameOver.ppm");
	playerImage = ppm6GetImage("images/player.ppm");
	background = ppm6GetImage("images/background.ppm");
	//
	// Start Screen
	glGenTextures(1, &startScreenId);
	int w = startScreen->width;
	int h = startScreen->height;
	glBindTexture(GL_TEXTURE_2D, startScreenId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, startScreen->data);
	//
	// Game Over
	glGenTextures(1, &gameOverId);
	w = gameOver->width;
	h = gameOver->height;
	glBindTexture(GL_TEXTURE_2D, gameOverId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, gameOver->data);
	//
	// Player Texture
	glGenTextures(1, &playerTextureId);
	w = playerImage->width;
	h = playerImage->height;
	glBindTexture(GL_TEXTURE_2D, playerTextureId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, playerImage->data);
	//
	// Background
	glGenTextures(1, &backgroundId);
	w = background->width;
	h = background->height;
	glBindTexture(GL_TEXTURE_2D, backgroundId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, background->data);
}*/

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
	g.score = g.ship.radius;

	cout << g.score << " = g.score (start)\n";

	//g.ship.radius = 40.0;
	//build 30 asteroids...
	for (int j=0; j<30; j++)
	{
		Asteroid *a = new Asteroid;
		a->nverts = 8;
		a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.8 * g.ship.radius  );
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

		Flt tmpx = xres >> 2;
		Flt tmpy = yres >> 2;
		if (a->pos[0] <= (3.0 * tmpx) &&
			a->pos[0] >= (tmpx) &&
			a->pos[1] <= (3.0 * tmpy) &&
			a->pos[1] >= (tmpy)
		   )//protect area near center from bigger asteroids
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
		//add to front of linked list
		a->next = g.ahead;
		if (g.ahead != NULL)
			g.ahead->prev = a;

		g.ahead = a;
		g.nasteroids++;
	}

	memset(keys, 0, 65536);
}

void initSounds(void)
{
	#ifdef USE_SOUND
	//FMOD_RESULT result;
	if (fmod_init()) {
		std::cout << "ERROR - fmod_init()\n" << std::endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/tick.wav", 0)) {
		std::cout << "ERROR - fmod_createsound()\n" << std::endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/drip.wav", 1)) {
		std::cout << "ERROR - fmod_createsound()\n" << std::endl;
		return;
	}
	fmod_setmode(0,FMOD_LOOP_OFF);
	//fmod_playsound(0);
	//fmod_systemupdate();
	#endif
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
	v[0] *= len * 2;
	v[1] *= len * 2;
}

void checkMouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
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
	if (savex != e->xbutton.x || savey != e->xbutton.y)
	{
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		int y = yres - e->xbutton.y;
		float dx = savex - g.ship.pos[0];
		float dy = y - g.ship.pos[1];
		float len = sqrt(dx * dx + dy * dy);

		g.ship.vel[0] = dx / len;
		g.ship.vel[1] = dy / len;
		normalize(g.ship.vel);
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
		if (key == XK_Shift_L || key == XK_Shift_R) {
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
			break;
		case XK_space:
			g.startScreen = 0;
			break;
	}

	return 0;
}

void addAsteroid ()
{
    Asteroid *a = new Asteroid;
    a->nverts = 8;
    a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.5 * g.ship.radius  );

    //cout << "Creating Asteroid with radius: " << a->radius << endl;
	//cout << "Current Ship Radius: " << g.ship.radius << endl;

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


    Flt tmpx = xres >> 2;
    Flt tmpy = yres >> 2;
    if (a->pos[0] <= (3.0 * tmpx) &&
	    a->pos[0] >= (tmpx) &&
	    a->pos[1] <= (3.0 * tmpy) &&
	    a->pos[1] >= (tmpy)
       )//protect area near center from bigger asteroids
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
    }    else    {
	a->color[0] = 0.3;
	a->color[1] = 0.4;
	a->color[2] = 0.5;
    }

    a->vel[0] = (Flt)(rnd()*2.0-1.0);
    a->vel[1] = (Flt)(rnd()*2.0-1.0);
    //add to front of linked list
    a->next = g.ahead;
    if (g.ahead != NULL)
	g.ahead->prev = a;

    g.ahead = a;
    g.nasteroids++;
}

void deleteAsteroid(Asteroid *node)
{
	//remove a node from linked list
	if (!g.done)
	{
		if (node)
		{
			if (node->prev == NULL)
			{
				if (node->next == NULL)
				{
					g.ahead = NULL;
				}
				else
				{
					node->next->prev = NULL;
					g.ahead = node->next;
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

/*void renderStartScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//-------------------------------------------------------------------------
	//Draw the Earth
	glColor3fv(g.ship.color);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, startScreenId);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0, yres);
	glTexCoord2f(0,1); glVertex2f(0, 0);
	glTexCoord2f(1,1); glVertex2f(xres, 0);
	glTexCoord2f(1,0); glVertex2f(xres, yres);
	glEnd();
}
void renderGameOver()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//-------------------------------------------------------------------------
	//Draw the Earth
	glColor3fv(g.ship.color);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, gameOverId);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0, yres);
	glTexCoord2f(0,1); glVertex2f(0, 0);
	glTexCoord2f(1,1); glVertex2f(xres, 0);
	glTexCoord2f(1,0); glVertex2f(xres, yres);
	glEnd();
}*/


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
	while (a) {
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
				fmod_playsound(0);
				Asteroid *savea = a->next;

				cout << g.score << " g.score (before add)\n"
				   	<< g.ship.radius << " ship radius\n"
					<< a->radius << " asteroid radius\n";

				if (a->radius > 0)
					g.score += 0.5 * a->radius;

				cout << g.score << " g.score (after add)\n"
				   	<< g.ship.radius << " ship radius\n\n";

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
	cout << (float)xres << " xres\n";
	cout << (float)yres << " yres\n";
	cout << std::dec << (g.ship.pos) << " g.ship.pos\n";
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

/*void updateScore()
{
	Rect r;
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ffff00, "Score: %i", (int)g.score);
	cout << "g.score:" << g.score << endl;
}*/

/*void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3fv(g.ship.color);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, backgroundId);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0, yres);
	glTexCoord2f(0,1); glVertex2f(0, 0);
	glTexCoord2f(1,1); glVertex2f(xres, 0);
	glTexCoord2f(1,0); glVertex2f(xres, yres);
	glEnd();
	glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
	glBindTexture(GL_TEXTURE_2D, playerTextureId);
	glBegin(GL_TRIANGLE_FAN);
	float x = (float)g.ship.radius * cos(999 * PI / 180.f);
	float y = (float)g.ship.radius * sin(999 * PI / 180.f);
	for (int i = 0; i <= 1000; i++)
	{
		glVertex2f(x, y);
		glTexCoord2f(x, y);
		x = (float)g.ship.radius * cos(i * PI / 180.f);
		y = (float)g.ship.radius * sin(i * PI / 180.f);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//Draw the asteroids
	Asteroid *a = g.ahead;
	while (a)
	{
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
	updateScore();
}*/
