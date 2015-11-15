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

Ppmimage *startScreen=NULL;
Ppmimage *gameOver=NULL;
Ppmimage *playerImage=NULL;
Ppmimage *background=NULL;
GLuint startScreenId;
GLuint gameOverId;
GLuint playerTextureId;
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
