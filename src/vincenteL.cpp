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
#include <GL/glx.h>
//#include "ppm.h"
//#include "log.h"
#include "structs.h"
extern "C"
{
	#include "fonts.h"
}

//struct Vec {
//	float x, y, z;
//};


struct Shape {
	float width, height;
	float radius;
	Vec center;
};


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

void checkSBoost()
{
	struct timespec sb;
	clock_gettime(CLOCK_REALTIME, &sb);
}

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
			checkSBoost();
			g.sBoost ^= 1;
			break;

	}
	
	return 0;
}

extern void showSBoost()
{
	Rect boost1;
	boost1.bot = yres - (yres*.90) + 30;
	boost1.left = 20;
	boost1.center = 0;
	ggprint16(&boost1, 16, 0x00ffff00, "Press b for Boost");
	
	Rect boost2;
	boost2.bot = yres - (yres*.90);
	boost2.left = 20;
	boost2.center = 0;
	ggprint16(&boost2, 16, 0x00ffff00, "Boost:");
}

extern void renderBoostBar()
{
	Shape s;
	float w;
	float h;
	
	if(g.sBoost)
	{
		glColor3ub(90,140,90);
		s.width = 10;
		s.height = 10;
		s.center[0] = 100; 
		s.center[1] = yres - (yres*.90) + 10;
		glPushMatrix();
		glTranslatef(s.center[0], s.center[1], s.center[2]);
		w = s.width;
		h = s.height;
		glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
		glVertex2i(-w,-h);
		glEnd();
		glPopMatrix();
		
		Rect boost3;
		boost3.bot = yres - 40;
		boost3.left = xres/2;
		boost3.center = 0;
		ggprint16(&boost3, 16, 0x00ffffff, "Boost Activated!!!!");
	}
	else
	{
		glColor3ub(90,140,90);
		s.width = 10;
		s.height = 10;
		s.center[0] = 100; 
		s.center[1] = yres - (yres*.90) + 10;
		glPushMatrix();
		glTranslatef(s.center[0], s.center[1], s.center[2]);
		w = s.width;
		h = s.height;
		glBegin(GL_LINE_STRIP);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
		glVertex2i(-w,-h);
		glEnd();
		glPopMatrix();
	}
}

void updateCamera()
{ 
	showSBoost();
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
