/*
	Vincente Lara's responsibility
	create the 2D camera that follows the character
*/

#include <iostream>
#include <cstdlib>
#include <cstring>
//#include <unistd.h>
//#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/keysym.h>
//#include <GL/glx.h>
//#include "ppm.h"
//#include "log.h"
#include "structs.h"

/*void checkSBoost()
{
	if( key == XK_b )
	{
		g.ship.vel[0] *= 4;
		g.ship.vel[1] *= 4;
		std::cout << "velocity increased";
	}	
}*/

extern double timeDiff(struct timespec *start, struct timespec *end);

int vinceCheckKeys(XEvent *e)
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
		case XK_b:
			//g.sBoost = 1;
			g.sBoost ^= 1;
			break;

	}
	
	return 0;
}

void updateCamera()
{ 
	std::cout << g.sBoost << " :sBoost\n";
	//cout << (float)xres << " xres\n";
	//cout << (float)yres << " yres\n";
	//cout << std::dec << (g.ship.pos) << " g.ship.pos\n";
	bool Move = false;
	//void checkSBoost();
	if(g.sBoost)
	{
		struct timespec ct;
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&ct, &bt);
		if (ts > 2.5)
		{
			//g.sBoost = 0;
		}	
	}
	
	if(g.ship.pos[0] < (float)(xres)*.65+.1 && g.ship.pos[0] > (float)(xres)*.35-.1
		&& g.ship.pos[1] < (float)(yres)*.65+.1 && g.ship.pos[1] > (float)(yres)*.35-.1)
		{
			//Update ship position
			if(g.sBoost)
			{
				g.ship.pos[0] += g.ship.vel[0] * 2;
				g.ship.pos[1] += g.ship.vel[1] * 2;
				Move = false;
			}
			else
			{	
				g.ship.pos[0] += g.ship.vel[0];
				g.ship.pos[1] += g.ship.vel[1];
				Move = false;
			}
		}
		else
		{
				Move = true;
				if(g.ship.pos[0] > (float)(xres)*.65+.01)
					g.ship.pos[0] = (float)(xres)*.65;
				if(g.ship.pos[0] < (float) (xres)*.35-.01)
					g.ship.pos[0] = (float)(xres)*.35;
				if(g.ship.pos[1] > (float)(yres)*.65+.01)
					g.ship.pos[1] = (float)(yres)*.65;
				if(g.ship.pos[1] < (float)(yres)*.35-.01)
					g.ship.pos[1] = (float)(yres)*.35;
					
					
		}

	Asteroid *a = g.ahead;
	if (Move)
	{
		while (a)
		{
			if(g.sBoost)
			{
				a->pos[0] -= g.ship.vel[0] * 2 * 2;
				a->pos[1] -= g.ship.vel[1] * 2 * 2;
				a = a->next;
			}
			else
			{
			a->pos[0] -= g.ship.vel[0] * 2;
			a->pos[1] -= g.ship.vel[1] * 2;
			a = a->next;
			}
		}
	}
}


/*extern int keys[65536];
extern int xres, yres;

//X Windows variables
extern Display *dpy;
extern Window win;
extern GLXContext glc;

extern void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "Super Dank Space Adventure");
}

extern void setup_screen_res(const int w, const int h)
{
    xres = w;
    yres = h;
}

extern int checkKeys(XEvent *e)
{
	//keyboard input?
	static int shift=0;
	int key = XLookupKeysym(&e->xkey, 0);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift=0;
		return 0;
	}
	if (e->type == KeyPress) {
		keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift=1;
			return 0;
		}
	} else {
		return 0;
	}
	if (shift){}
	switch(key) {
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

extern void initXWindows(void)
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
    XSetWindowAttributes swa;
    setup_screen_res(xres, yres);
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        std::cout << "\n\tcannot connect to X server" << std::endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
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
}*/
