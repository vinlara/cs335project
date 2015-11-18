#ifndef ERICS_H
#define ERICS_H

extern void init();
extern void asteroidRadiusSpeed( Asteroid *a );
extern void shipRadiusSpeed();
extern void addAsteroid();
extern void deleteAsteroid();
extern int smithCheckKeys( XEvent *e );

#endif
