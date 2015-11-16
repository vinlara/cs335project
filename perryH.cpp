#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <dirent.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <vector>
#include "ppm.h"
#include "structs.h"
using namespace std;
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

void loadFiles()
{
	struct dirent *entry;
	DIR *dp = opendir("./images/");
	g.nimages = 0;
	if (dp) {
		while ((entry = readdir(dp)) != NULL && g.nimages < MAX_IMAGES)
		{
			if (strstr(entry->d_name, ".png"))
			{
				char t1[256];
				char t2[256];
				char t3[256] = "images/";
				strcpy(t2, entry->d_name);
				int slen = strlen(t2);
				t2[slen - 4] = '\0';
				strcat(t2, ".ppm");
				sprintf(t1, "convert images/%s images/%s", entry->d_name, t2);
				system(t1);
				strcat(t3, t2);
				strcpy(g.imageName[g.nimages++], t3);
				strcpy(g.imageTemp[g.nt++], t3);
			}
		}
		closedir(dp);
	}
}

void cleanupTempFiles()
{
	for (int i = 0; i<g.nt; i++)
	{
		remove(g.imageTemp[i]);
	}
}

void initTextures(void)
{
	//
	// Start Screen
	startScreen = ppm6GetImage("images/screen_begin.ppm");
	glGenTextures(1, &startScreenId);
	int w = startScreen->width;
	int h = startScreen->height;
	glBindTexture(GL_TEXTURE_2D, startScreenId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, startScreen->data);
	//
	// Game Over
	gameOver = ppm6GetImage("images/screen_gameover.ppm");
	glGenTextures(1, &gameOverId);
	w = gameOver->width;
	h = gameOver->height;
	glBindTexture(GL_TEXTURE_2D, gameOverId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, gameOver->data);
	//
	// Player Texture
	playerImage = ppm6GetImage("images/texture_player.ppm");
	glGenTextures(1, &playerTextureId);
	w = playerImage->width;
	h = playerImage->height;
	glBindTexture(GL_TEXTURE_2D, playerTextureId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, playerImage->data);
	//
	// Background
	background = ppm6GetImage("images/background.ppm");
	glGenTextures(1, &backgroundId);
	w = background->width;
	h = background->height;
	glBindTexture(GL_TEXTURE_2D, backgroundId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, background->data);
	//
	// Particle Textures
	for (int i = 0; i < 6; ++i)
	{
		ppm6CleanupImage(particleImage);
		stringstream tempStr;
		tempStr<<(i + 1);
		string str = tempStr.str();
		char t1[256] = "images/texture_enemy_";
		const char* t2 = str.c_str();
		char t3[256] = ".ppm";
		strcat(t1, t2);
		strcat(t1, t3);
		particleImage = ppm6GetImage(t1);
		glGenTextures(1, &particleTextureId[i]);
		w = particleImage->width;
		h = particleImage->height;
		glBindTexture(GL_TEXTURE_2D, particleTextureId[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, particleImage->data);
	}
}

void renderStartScreen()
{
	Rect startScreen;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, startScreenId);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0, yres);
	glTexCoord2f(0,1); glVertex2f(0, 0);
	glTexCoord2f(1,1); glVertex2f(xres, 0);
	glTexCoord2f(1,0); glVertex2f(xres, yres);
	glEnd();
	startScreen.bot = yres / 5;
	startScreen.left = xres / 2;
	startScreen.center = xres / 2;
	ggprint16(&startScreen, 16, 0x00ffff00, "Press the 'Enter' Key to Start");
}

void renderGameOver()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
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
	glColor3f(1.0, 1.0, 1.0);
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
		glBindTexture(GL_TEXTURE_2D, particleTextureId[a->textureId]);
		glPushMatrix();
		glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
		glBegin(GL_TRIANGLE_FAN);
		float x = (float)a->radius * cos(999 * PI / 180.f);
		float y = (float)a->radius * sin(999 * PI / 180.f);
		for (int i = 0; i <= 1000; i++)
		{
			glVertex2f(x, y);
			glTexCoord2f(x, y);
			x = (float)a->radius * cos(i * PI / 180.f);
			y = (float)a->radius * sin(i * PI / 180.f);
		}
		glEnd();
		glPopMatrix();
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
		shipRadiusSpeed();
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
		case XK_Return:
			if (g.startScreen)
			{
				g.startScreen = 0;
			}
			break;
	}
	return 0;
}
