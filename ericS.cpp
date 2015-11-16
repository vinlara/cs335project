/*  Eric Smith
    ericS.cpp

	CS 335 Software Engineering Project
	Super Dank Space Adventure

	Group Members:
	Eric Smith
	Erik Juarez
	Perry Huynh
	Vincente Lara
*/
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "structs.h"
using namespace std;

void addAsteroid () 
{
    Asteroid *a = new Asteroid;
    a->nverts = 8;
    a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.5 * g.ship.radius  );
    a->textureId = rand() % 20;
    //cout << "Created asteroid with Texture ID: " << a->textureId << endl;
    //cout << a->radius << " a radius   ship radius " << g->ship.radius << endl;
    
    Flt r2 = a->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)a->nverts;
    for (int i=0; i<a->nverts; i++) 
    {
        a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
        a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
        angle += inc;
    }
    a->pos[0] = (Flt)(rand() % xres);
    a->pos[1] = (Flt)(rand() % yres);
    a->pos[2] = 0.0f;
    a->angle = 0.0;
    a->rotate = rnd() * 4.0 - 2.0;

    Flt tmpx = xres >> 2;
    Flt tmpy = yres >> 2;
    if ( a->pos[0] < (3.0 * tmpx) &&
	    a->pos[0] > (tmpx) &&
	    a->pos[1] < (3.0 * tmpy) &&
	    a->pos[1] > (tmpy)
       )//protect area near center from bigger asteroids
    {
	if (a->pos[0] < (2.0 * tmpx) &&
		a->pos[0] > tmpx
	   )//left half 
	{
	    a->pos[0] -= tmpx;
	    a->pos[2] = 0.0f;
	}
	
	else //right half 
	{
	    a->pos[0] += tmpx;
	    a->pos[2] = 0.0f;
	}
    }

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

    a->vel[0] = (Flt)(rnd()*2.0-1.0);
    a->vel[1] = (Flt)(rnd()*2.0-1.0);
    //add to front of linked list
    a->next = g.ahead;
    if (g.ahead != NULL)
    {
    	g.ahead->prev = a;
    }

    g.ahead = a;
    g.nasteroids++;
}

void deleteAsteroid(Asteroid *node)
{
    //remove a node from linked list
    if (!g.done)
    {
	if (node)
	{
	    if (node->prev == NULL)
	    {
		if (node->next == NULL)
		{
		    g.ahead = NULL;
		}
		else
		{
		    node->next->prev = NULL;
		    g.ahead = node->next;
		}
	    }
	    else
	    {
		if (node->next == NULL)
		{
		    node->prev->next = NULL;
		}
		else
		{
		    node->prev->next = node->next;
		    node->next->prev = node->prev;
		}
	    }
	    delete node;
	    node = NULL;
	}
    }
}

void asteroidRadiusSpeed(Asteroid *a)
{
    /*
    cout << "asteroidRadiusSpeed ftn ------------------------\n";
    cout << "axVel: " << a->vel[0] << endl;
    cout << "ayVel: " << a->vel[1] << endl;

    cout << a->radius << ": asteroid radius\n";
    */

    Flt radiusVel = ( ( 500.0 / ( a->radius + 100.0 ) ) - 1.5 );

    Flt xVel = a->vel[0];
    Flt yVel = a->vel[1];
    Flt speed = sqrt( ( xVel * xVel ) + ( yVel * yVel ) );
    Flt xRatio = ( xVel / speed ) * radiusVel;
    Flt yRatio = ( yVel / speed ) * radiusVel;
    
    a->vel[0] = xRatio;
    a->vel[1] = yRatio;

    //cout << "asteroidRadiusSpeed ftn##### AFTER Radius Change !!!!!!!!!!!!!!!!!\n";
    //cout << "axVel: " << a->vel[0] << endl;
    //cout << "ayVel: " << a->vel[1] << endl;
    
}

void shipRadiusSpeed()
{
    /*
    cout << "shipRadiusSpeed ftn&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
    cout << g.ship.vel[0] << " ShipXVel\n";
    cout << g.ship.vel[1] << " ShipYVel\n";
    
    cout << g.ship.radius << ": ship radius\n";
    */

    Flt radiusVel = ( ( 500.0 / ( g.ship.radius + 100.0 ) ) - 1.5 );

    Flt xVel = g.ship.vel[0];
    Flt yVel = g.ship.vel[1];
    Flt speed = sqrt( ( xVel * xVel ) + ( yVel * yVel ) );
    Flt xRatio = ( xVel / speed ) * radiusVel;
    Flt yRatio = ( yVel / speed ) * radiusVel;

    g.ship.vel[0] = xRatio;
    g.ship.vel[1] = yRatio;
        
    //cout << "shipRadiusSpeed ftn&&&&&&&&&&& AFTER CHANGE &&&&&&&&&&&&&&&\n";
    //cout << g.ship.vel[0] << " ShipXVel\n";
    //cout << g.ship.vel[1] << " ShipYVel\n";

}
