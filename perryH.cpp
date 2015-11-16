#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <dirent.h>
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "ppm.h"
#include "structs.h"
#include "perryH.h"
extern "C"
{
	#include "fonts.h"
}

Ppmimage *ppm = NULL;
GLuint startScreenId;
GLuint helpScreenId;
GLuint gameOverId;
GLuint backgroundId;
GLuint playerTextureId;
GLuint particleTextureId[10];

extern void loadTempFiles()
{
	struct dirent *entry;
	DIR *dp = opendir("./images/");
	g.nimages = 0;

	if (dp)
	{
		std::cout << "\nentering directory: images/\n" << std::endl;
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

	std::cout << "successfully converted files:" << std::endl;
	for (int i = 0; i < g.nt; ++i)
	{
		std::cout << g.imageTemp[i] << std::endl;
	}

	std::cout << std::endl;
}

extern void cleanupTempFiles()
{
	for (int i = 0; i<g.nt; i++)
	{
		remove(g.imageTemp[i]);
		std::cout << "successfully removed file: " << g.imageTemp[i] << std::endl;
	}
}

extern void initTextures(void)
{
	//
	// Start Screen
	ppm = ppm6GetImage("images/screen_begin.ppm");
	std::cout << "successfully loaded file: images/screen_begin.ppm" << std::endl;
	glGenTextures(1, &startScreenId);
	int w = ppm->width;
	int h = ppm->height;
	glBindTexture(GL_TEXTURE_2D, startScreenId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);
	//
	// Help Screen
	ppm6CleanupImage(ppm);
	ppm = ppm6GetImage("images/screen_howtoplay.ppm");
	std::cout << "successfully loaded file: images/screen_howtoplay.ppm" << std::endl;
	glGenTextures(1, &helpScreenId);
	w = ppm->width;
	h = ppm->height;
	glBindTexture(GL_TEXTURE_2D, helpScreenId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);
	//
	// Game Over
	ppm = ppm6GetImage("images/screen_gameover.ppm");
	std::cout << "successfully loaded file: images/screen_gameover.ppm" << std::endl;
	glGenTextures(1, &gameOverId);
	w = ppm->width;
	h = ppm->height;
	glBindTexture(GL_TEXTURE_2D, gameOverId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);
	//
	// Background
	ppm6CleanupImage(ppm);
	ppm = ppm6GetImage("images/background.ppm");
	std::cout << "successfully loaded file: images/background.ppm" << std::endl;
	glGenTextures(1, &backgroundId);
	w = ppm->width;
	h = ppm->height;
	glBindTexture(GL_TEXTURE_2D, backgroundId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);
	//
	// Player Texture
	ppm6CleanupImage(ppm);
	ppm = ppm6GetImage("images/texture_player.ppm");
	std::cout << "successfully loaded file: images/texture_player.ppm" << std::endl;
	glGenTextures(1, &playerTextureId);
	w = ppm->width;
	h = ppm->height;
	glBindTexture(GL_TEXTURE_2D, playerTextureId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);
	//
	// Particle Textures
	for (int i = 0; i < 6; ++i)
	{
		std::stringstream tempStr;
		tempStr<<(i + 1);
		std::string str = tempStr.str();
		char t1[256] = "images/texture_enemy_";
		const char* t2 = str.c_str();
		const char* t3 = ".ppm";
		strcat(t1, t2);
		strcat(t1, t3);

		ppm6CleanupImage(ppm);
		ppm = ppm6GetImage(t1);
		std::cout << "successfully loaded file: " << t1 << std::endl;
		glGenTextures(1, &particleTextureId[i]);
		w = ppm->width;
		h = ppm->height;
		glBindTexture(GL_TEXTURE_2D, particleTextureId[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);
	}
	std::cout << std::endl;
}

extern void renderStartScreen()
{
	Rect startScreen;
	Rect help;

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
	ggprint16(&startScreen, 16, 0x00ffffff, "Press 'Enter' to Start");

	help.bot = yres / 5 - 30;
	help.left = xres / 2;
	help.center = xres/ 2;
	ggprint16(&help, 16, 0x00ffffff, "Press 'h' for Help");

	std::cout << "rendering screen_start" << std::endl;
}

extern void renderHelpScreen()
{
	Rect helpRect;

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, helpScreenId);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0, yres);
	glTexCoord2f(0,1); glVertex2f(0, 0);
	glTexCoord2f(1,1); glVertex2f(xres, 0);
	glTexCoord2f(1,0); glVertex2f(xres, yres);
	glEnd();

	helpRect.bot = yres / 2 + 250;
	helpRect.left = xres / 2;
	helpRect.center = xres / 2;
	ggprint16(&helpRect, 16, 0x00ffffff, "Press 'BackSpace' to go back");

	std::cout << "rendering screen_howtoplay" << std::endl;
}

extern void renderGameOver()
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

	std::cout << "rendering screen_gameover" << std::endl;
}

extern void updateScore()
{
	Rect score;
	score.bot = yres - 40;
	score.left = 20;
	score.center = 0;
	ggprint16(&score, 16, 0x00ffff00, "Score: %i", (int)g.score);
}

extern void render()
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
	std::cout << "rendering game" << std::endl;
}

extern void normalize(Vec v)
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

extern void checkMouse(XEvent *e)
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
		//normalize(g.ship.vel);
		shipRadiusSpeed();
		normalize(g.ship.vel);
		return;
	}
}

extern int checkKeys(XEvent *e)
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
	if (shift)
	{
	}
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
		case XK_h:
			if (g.startScreen)
			{
				g.helpScreen = 1;
			}
			break;
		case XK_BackSpace:
			if (g.helpScreen == 1)
			{
				g.helpScreen = 0;
			}
			break;
	}
	return 0;
}
