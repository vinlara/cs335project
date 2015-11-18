// Erik Juarez
// Lab 6

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


//Sources & Buffers
extern ALuint alSource;
extern ALuint alSource1;
extern ALuint alSource2;
extern ALuint alBuffer;
extern ALuint alBuffer1;
extern ALuint alBuffer2;

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

void init_sounds(){

	//Get started right here.
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
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
	alBuffer = alutCreateBufferFromFile("sounds/test.wav");
	alBuffer1 = alutCreateBufferFromFile("sounds/collision.wav");
	alBuffer2 = alutCreateBufferFromFile("sounds/gameplay.wav");
	//
	//Source refers to the sound.
//ALuint alSource;
	//Generate a source, and store it in a buffer.
	alGenSources(1, &alSource);
	alGenSources(1, &alSource1);
	alGenSources(1, &alSource2);
	alSourcei(alSource, AL_BUFFER, alBuffer);
	alSourcei(alSource1, AL_BUFFER, alBuffer1);
	alSourcei(alSource2, AL_BUFFER, alBuffer2);
	//Set volume and pitch to normal, no looping of sound.
	alSourcef(alSource, AL_GAIN, 1.0f);
	alSourcef(alSource, AL_PITCH, 1.0f);
	alSourcei(alSource, AL_LOOPING, AL_FALSE);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource1, AL_GAIN, 1.0f);
	alSourcef(alSource1, AL_PITCH, 1.0f);
	alSourcei(alSource1, AL_LOOPING, AL_FALSE);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting source\n");
	}

	alSourcef(alSource2, AL_GAIN, 1.0f);
	alSourcef(alSource2, AL_PITCH, 1.0f);
	alSourcei(alSource2, AL_LOOPING, AL_TRUE);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting source\n");
	}
}

void cleanup_sounds(){
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

void stop_playing(ALuint to_stop){
alSourceStop(to_stop);
}
