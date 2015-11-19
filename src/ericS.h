#ifndef ERICS_H
#define ERICS_H

extern void init();
extern void asteroidRadiusSpeed( Asteroid *a );
extern void shipRadiusSpeed();
extern void addAsteroid();
extern void deleteAsteroid();
extern void deleteAllAsteroid();
extern void remakeGame();
extern int smithCheckKeys( XEvent *e );
extern void stop_playing(ALuint);
extern ALuint alSource1;
extern ALuint alSource6;
extern void play_on_r();
extern int rcount;
extern bool gameovercont;

#endif
