// Erik Juarez
// Lab 6
/*
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include </usr/include/AL/alut.h>
*/

#include <iostream>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include </usr/include/AL/alut.h>
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
extern "C"
{
	#include "fonts.h"
}

using namespace std;

extern int bcount;
extern int ecount;
extern int wcount;

extern ALuint alSource;		//menu.wav
extern ALuint alSource1;	//gameplay.wav
extern ALuint alSource2;	//collision.wav
extern ALuint alSource3;	//badcollision.wav
extern ALuint alSource4;	//boost.wav
extern ALuint alSource5;	//gameover.wav
extern ALuint alSource6;	//gameovercont.wav
extern ALuint alSource7;	//enterkey.wav
extern ALuint alSource8;	//eraseplanets.wav
extern ALuint alSource9;	//warp.wav

extern ALuint alBuffer;
extern ALuint alBuffer1;
extern ALuint alBuffer2;
extern ALuint alBuffer3;
extern ALuint alBuffer4;
extern ALuint alBuffer5;
extern ALuint alBuffer6;
extern ALuint alBuffer7;
extern ALuint alBuffer8;
extern ALuint alBuffer9;

void init_sounds();
void play_on_b();
void play_on_e();
void play_on_w();
void stop_playing(ALuint);
void cleanup_sounds();
void rendergametitle();
void rendermorehelp();


void init_opengl(void)
{

	//OpenGL initialization
	glViewport(0, 0, 1280, 960);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, 1280, 0, 960, -1, 1);
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


void init_sounds()
{

	//Get started right here.
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) 
	{

		printf("ERROR: alutInit()\n");
	
	}
	//Clear error state.
	alGetError();
	//
	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
	//
	//Buffer holds the sound information.

//ALuint alBuffer;
	alBuffer = alutCreateBufferFromFile("sounds/menu.wav");
	alBuffer1 = alutCreateBufferFromFile("sounds/gameplay.wav");
	alBuffer2 = alutCreateBufferFromFile("sounds/collision.wav");
	alBuffer3 = alutCreateBufferFromFile("sounds/badcollision.wav");
	alBuffer4 = alutCreateBufferFromFile("sounds/boost.wav");
	alBuffer5 = alutCreateBufferFromFile("sounds/gameover.wav");
	alBuffer6 = alutCreateBufferFromFile("sounds/gameovercont.wav");
	alBuffer7 = alutCreateBufferFromFile("sounds/enterkey.wav");
	alBuffer8 = alutCreateBufferFromFile("sounds/eraseplanets.wav");
	alBuffer9 = alutCreateBufferFromFile("sounds/warp.wav");

	//Source refers to the sound.
//ALuint alSource;
	//Generate a source, and store it in a buffer.
	alGenSources(1, &alSource);
	alGenSources(1, &alSource1);
	alGenSources(1, &alSource2);
	alGenSources(1, &alSource3);
	alGenSources(1, &alSource4);
	alGenSources(1, &alSource5);
	alGenSources(1, &alSource6);
	alGenSources(1, &alSource7);
	alGenSources(1, &alSource8);
	alGenSources(1, &alSource9);

	alSourcei(alSource, AL_BUFFER, alBuffer);
	alSourcei(alSource1, AL_BUFFER, alBuffer1);
	alSourcei(alSource2, AL_BUFFER, alBuffer2);
	alSourcei(alSource3, AL_BUFFER, alBuffer3);
	alSourcei(alSource4, AL_BUFFER, alBuffer4);
	alSourcei(alSource5, AL_BUFFER, alBuffer5);
	alSourcei(alSource6, AL_BUFFER, alBuffer6);
	alSourcei(alSource7, AL_BUFFER, alBuffer7);
	alSourcei(alSource8, AL_BUFFER, alBuffer8);
	alSourcei(alSource9, AL_BUFFER, alBuffer9);

	//Set volume and pitch to normal, no looping of sound.
	alSourcef(alSource, AL_GAIN, 1.0f);
	alSourcef(alSource, AL_PITCH, 1.0f);
	alSourcei(alSource, AL_LOOPING, AL_TRUE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
	    printf("ERROR: setting source\n");
	}

	//But here I need looping sound
	alSourcef(alSource1, AL_GAIN, 1.0f);
	alSourcef(alSource1, AL_PITCH, 1.0f);
	alSourcei(alSource1, AL_LOOPING, AL_TRUE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource2, AL_GAIN, 1.0f);
	alSourcef(alSource2, AL_PITCH, 1.0f);
	alSourcei(alSource2, AL_LOOPING, AL_FALSE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource3, AL_GAIN, 1.0f);
	alSourcef(alSource3, AL_PITCH, 1.0f);
	alSourcei(alSource3, AL_LOOPING, AL_FALSE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource4, AL_GAIN, 1.0f);
	alSourcef(alSource4, AL_PITCH, 1.0f);
	alSourcei(alSource4, AL_LOOPING, AL_TRUE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource5, AL_GAIN, 1.0f);
	alSourcef(alSource5, AL_PITCH, 1.0f);
	alSourcei(alSource5, AL_LOOPING, AL_FALSE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource6, AL_GAIN, 1.0f);
	alSourcef(alSource6, AL_PITCH, 1.0f);
	alSourcei(alSource6, AL_LOOPING, AL_TRUE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource7, AL_GAIN, 1.0f);
	alSourcef(alSource7, AL_PITCH, 1.0f);
	alSourcei(alSource7, AL_LOOPING, AL_FALSE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource8, AL_GAIN, 1.0f);
	alSourcef(alSource8, AL_PITCH, 1.0f);
	alSourcei(alSource8, AL_LOOPING, AL_FALSE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource9, AL_GAIN, 1.0f);
	alSourcef(alSource9, AL_PITCH, 1.0f);
	alSourcei(alSource9, AL_LOOPING, AL_FALSE);
	
	if (alGetError() != AL_NO_ERROR) 
	{
		printf("ERROR: setting source\n");
	}

}


void cleanup_sounds()
{

	//Cleanup.
	//First delete the source.
	alDeleteSources(1, &alSource);
	alDeleteSources(1, &alSource1);
	//Delete the buffer.
	alDeleteBuffers(1, &alBuffer);
	alDeleteBuffers(1, &alBuffer1);
	//Close out OpenAL itself.
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);

}


void stop_playing(ALuint to_stop)
{

	alSourceStop(to_stop);

}


void play_on_b()
{

	if(bcount%2 == 0)
	{

		stop_playing(alSource4);
	
	}

	if(bcount%2 != 0)
	{

	    alSourcePlay(alSource4);
	
	}

}


void play_on_e()
{

	if(ecount > 0)
	{

		alSourcePlay(alSource8);
	
	}

}


void play_on_w()
{

	if(wcount > 0)
	{

		alSourcePlay(alSource9);
	
	}

}


void rendergametitle()
{

	Rect welcome;
	welcome.bot = yres - 40;
	welcome.left = xres / 2;
	welcome.center = xres / 2;
	ggprint16(&welcome, 16, 0x00ffffff, "Welcome to.....");

}


void rendermorehelp()
{

	Rect l1;
	l1.bot = yres - ((yres/3));
	l1.left = xres / 2;
	l1.center = xres / 2;
	ggprint16(&l1, 16, 0x00ffffff, "Move the mouse: control your planet.");

	Rect l2;
	l2.bot = yres - ((yres/3)+60);
	l2.left = xres / 2;
	l2.center = xres / 2;
	ggprint16(&l2, 16, 0x00ffffff, "Press button B: Activate boost (move faster).");

	Rect l3;
	l3.bot = yres - ((yres/3)+120);
	l3.left = xres / 2;
	l3.center = xres / 2;
	ggprint16(&l3, 16, 0x00ffffff, "Press button E: Erases enemy planets to give you a quick advantage.");

	Rect l4;
	l4.bot = yres - ((yres/3)+180);
	l4.left = xres / 2;
	l4.center = xres / 2;
	ggprint16(&l4, 16, 0x00ffffff, "Press button W: Warps your planet around the screen.");

	Rect l5;
	l5.bot = yres - ((yres/3)+240);
	l5.left = xres / 2;
	l5.center = xres / 2;
	ggprint16(&l5, 16, 0x00ffffff, "Press button P: Pauses the other planets.");

	Rect l6;
	l6.bot = yres - ((yres/3)+300);
	l6.left = xres / 2;
	l6.center = xres / 2;
	ggprint16(&l6, 16, 0x00ffffff, "Feeling lost in space? Press CTRL to locate your planet.");

}
