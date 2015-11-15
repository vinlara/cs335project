#include <cmath>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "ppm.h"
#include "structs.h"
extern "C" {
	#include "fonts.h"
}

Ppmimage *startScreen = NULL;
Ppmimage *gameOver = NULL;
Ppmimage *playerImage = NULL;
Ppmimage *particleImage = NULL;
Ppmimage *background = NULL;
GLuint startScreenId;
GLuint gameOverId;
GLuint playerTextureId;
GLuint particleTextureId[20];
GLuint backgroundId;

void initTextures(void)
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
}

void renderStartScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
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
	glBindTexture(GL_TEXTURE_2D, gameOverId);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0, yres);
	glTexCoord2f(0,1); glVertex2f(0, 0);
	glTexCoord2f(1,1); glVertex2f(xres, 0);
	glTexCoord2f(1,0); glVertex2f(xres, yres);
	glEnd();
}

void updateScore()
{
	Rect r;
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ffff00, "Score: %i", (int)g.score);
}

void render()
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
		//glBindTexture(GL_TEXTURE_2D, particleTextureId[a->textureId]);
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
	static int savex = 0;
	static int savey = 0;
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
			if (g.startScreen)
			{
				g.startScreen = 0;
			}
			break;
	}
	return 0;
}
