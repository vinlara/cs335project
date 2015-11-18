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

int pause = 0;
float temp1[30];
float temp2[30];
int showC = 0;
int slowM = 0;

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
int vinceCheckKeys(XEvent *e);

void checkSBoost()
{
	struct timespec sb;
	clock_gettime(CLOCK_REALTIME, &sb);
	g.sBoost ^= 1;
}

void spaceWarp()
{
	//if(g.ship.vel[0] > 0 && g.ship.vel[1] > 0)
	//{
		g.ship.pos[0] += rnd() * ((float)(xres)*.65+.1) + (float(xres)*.35-.1);
		g.ship.pos[1] += rnd() * ((float)(yres)*.65+.1) + (float(yres)*.35-.1);
		//g.ship.vel[0] += 10 + rnd();
		//g.ship.vel[1] += 10 + rnd();
	//}
	Rect boost3;
	boost3.bot = yres - 40;
	boost3.left = xres/2;
	boost3.center = 0;
	ggprint16(&boost3, 16, 0x00ffffff, "Space Warp!!!!");
		
}

void pauseMove()
{
	pause ^= 1;
	if(pause)
	{
		Asteroid *b = g.ahead;
		while (b)
			{
				int i = 0;
				temp1[i] = b->vel[0];
				temp2[i] = b->vel[1];
				b->vel[0] = 0;
				b->vel[1] = 0;
				b = b->next;
				i++;
			}	
		g.ship.vel[0] = 0;
		g.ship.vel[1] = 0;
	}
	else if(!pause)
	{
		Asteroid *b = g.ahead;
		while (b)
		{
			int i = 0;
			b->vel[0] = temp1[i];
			b->vel[1] = temp2[i];
			b = b->next;
			i++;
		}
	}
}

void explReset()
{
	Asteroid *c = g.ahead;
	g.ship.pos[0] = (float)(xres)/2;
	g.ship.pos[1] = (float)(yres)/2;
	while (c)
		{
			c->pos[0] = 0;
			c->pos[1] = 0;
			c = c->next;
		}
}

void showPlayer()
{
	Rect boost1;
	boost1.bot = g.ship.pos[1] + g.ship.radius + 40;
	boost1.left = g.ship.pos[0] - 25;
	boost1.center = 0;
	ggprint10(&boost1, 16, 0x00ffff00, "PLAYER");
}

void slowMo()
{
	slowM ^= 1;
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
			//g.sBoost ^= 1;
			break;
		case XK_w:
			spaceWarp();
			break;
		case XK_p:
			pauseMove();
			break;
		case XK_e:
			explReset();
			break;
		case XK_Control_L:
			showC ^= 1;
			break;
		case XK_s:
			slowMo();

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
	
	Rect boost3;
	boost3.bot = yres - (yres*.90) - 30;
	boost3.left = 20;
	boost3.center = 0;
	ggprint16(&boost3, 16, 0x00ffff00, "press control to show position");
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
	
	if(showC)
	{
		showPlayer();
		//glClear(GL_COLOR_BUFFER_BIT);
		glColor3ub(150, 150, 150);
		glPushMatrix();
		glTranslatef(g.ship.pos[0], g.ship.pos[1] + g.ship.radius + 2, 1.0f);
		glBegin(GL_LINES);
		glVertex2i(0, 2);
		glVertex2i(0, 30);
		glVertex2i(0, 2);
		glVertex2i(5, 10);
		glVertex2i(0, 2);
		glVertex2i(-5, 10);
		glEnd();
		glPopMatrix();
	}
	
	
	/*
	glPushMatrix();
	glColor3ub(90, 140, 90);
	//glTranslatef(g.ship.pos[0] + g.ship.radius, g.ship.pos[1] + g.ship.radius, g.ship.pos[2]);
	//if(g.ship.vel[0] > 
	glTranslatef(g.ship.pos[0] + g.ship.vel[0] + float(g.ship.radius), g.ship.pos[1] + g.ship.vel[1] + float(g.ship.radius), g.ship.pos[2]);
	//glRotatef(g.ship.vel[0], g.ship.radius, g.ship.radius, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(10.0f, 10.0f);
	glVertex2f(0.0f, 30.0f);
	glVertex2f(-10.0f,10.0f);
	glEnd();
	glPopMatrix();
	*/
	
}

void updateCamera()
{ 
	showSBoost();
	std::cout << g.sBoost << " :sBoost\n";
	//cout << (float)xres << " xres\n";
	//cout << (float)yres << " yres\n";
	//cout << std::dec << (g.ship.pos) << " g.ship.pos\n";
	bool Move = false;
	//if(pause)
	//{
	//	Move = true;
	//}
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
			else if(slowM)
			{
				g.ship.pos[0] += g.ship.vel[0] * .5;
				g.ship.pos[1] += g.ship.vel[1] * .5;
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
			else if(slowM)
			{
				a->pos[0] -= g.ship.vel[0];
				a->pos[1] -= g.ship.vel[1];
				a = a->next;
			}				
			else if(pause)
			{
				g.ship.vel[0] = 0;
				g.ship.vel[1] = 0;
				a->pos[0] = a->pos[0];
				a->pos[1] = a->pos[1];
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
