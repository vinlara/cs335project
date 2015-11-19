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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include "structs.h"
#include "ericS.h"
using namespace std;


void init()
{

    cout << g.score << " = g.score (start)\n";

    //build 30 asteroids...
    for ( int j=0; j<30; j++ )
    {
        <<<<<<< HEAD
            Asteroid *a = new Asteroid;
        a->nverts = 8;
        a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.8 * g.ship.radius  );
        a->textureId = rand() % 6;
        Flt r2 = a->radius / 2.0;
        Flt angle = 0.0f;
        Flt inc = (PI * 2.0) / (Flt)a->nverts;

        for (int i=0; i<a->nverts; i++)
        {
            a->vert[i][0] = sin( angle ) * ( r2 + rnd() * a->radius );
            a->vert[i][1] = cos( angle ) * ( r2 + rnd() * a->radius );
            angle += inc;
        }

        a->pos[0] = ( Flt ) ( rand() % xres );
        a->pos[1] = ( Flt ) ( rand() % yres );
        a->pos[2] = 0.0f;
        a->angle = 0.0;
        a->rotate = rnd() * 4.0 - 2.0;

        //protect area near center from bigger asteroids
        Flt tmpx = xres >> 2;
        Flt tmpy = yres >> 2;

        if ( a->pos[0] <= ( 3.0 * tmpx ) &&
                a->pos[0] >= tmpx &&
                a->pos[1] <= ( 3.0 * tmpy ) &&
                a->pos[1] >= tmpy
           )
        {

            if (a->pos[0] <= ( 2.0 * tmpx ) &&
                    a->pos[0] > tmpx
               ) //left half
            {
                a->pos[0] -= 1.25 * tmpx;
                a->pos[2] = 0.0f;
            }

            else //right half
            {
                a->pos[0] += 1.25 * tmpx;
                a->pos[2] = 0.0f;
            }
        }

        if ( a->radius < g.ship.radius )
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

        a->vel[0] = ( Flt ) ( rnd() * 2.0 - 1.0 );
        a->vel[1] = ( Flt ) ( rnd() * 2.0 - 1.0 );

        asteroidRadiusSpeed(a);

        //add to front of linked list
        a->next = g.ahead;

        if ( g.ahead != NULL )
        {
            g.ahead->prev = a;
        }

        g.ahead = a;
        g.nasteroids++;
        =======

            Asteroid *a = new Asteroid;
        a->nverts = 8;
        a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.8 * g.ship.radius  );
        a->textureId = rand() % 6;
        Flt r2 = a->radius / 2.0;
        Flt angle = 0.0f;
        Flt inc = (PI * 2.0) / (Flt)a->nverts;

        for (int i=0; i<a->nverts; i++)
        {

            a->vert[i][0] = sin( angle ) * ( r2 + rnd() * a->radius );
            a->vert[i][1] = cos( angle ) * ( r2 + rnd() * a->radius );
            angle += inc;

        }

        a->pos[0] = ( Flt ) ( rand() % xres );
        a->pos[1] = ( Flt ) ( rand() % yres );
        a->pos[2] = 0.0f;
        a->angle = 0.0;
        a->rotate = rnd() * 4.0 - 2.0;

        //protect area near center from bigger asteroids
        Flt tmpx = xres >> 2;
        Flt tmpy = yres >> 2;

        if ( a->pos[0] <= ( 3.0 * tmpx ) &&
                a->pos[0] >= tmpx &&
                a->pos[1] <= ( 3.0 * tmpy ) &&
                a->pos[1] >= tmpy
           )
        {

            if (a->pos[0] <= ( 2.0 * tmpx ) &&
                    a->pos[0] > tmpx
               ) //left half
            {

                a->pos[0] -= 1.25 * tmpx;
                a->pos[2] = 0.0f;

            }

            else //right half
            {

                a->pos[0] += 1.25 * tmpx;
                a->pos[2] = 0.0f;

            }

        }

        if ( a->radius < g.ship.radius )
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

        a->vel[0] = ( Flt ) ( rnd() * 2.0 - 1.0 );
        a->vel[1] = ( Flt ) ( rnd() * 2.0 - 1.0 );

        asteroidRadiusSpeed(a);

        //add to front of linked list
        a->next = g.ahead;

        if ( g.ahead != NULL )
        {

            g.ahead->prev = a;

        }

        g.ahead = a;
        g.nasteroids++;

        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
    }

    memset(keys, 0, 65536);

}


void addAsteroid ()
{

    Asteroid *a = new Asteroid;
    a->nverts = 8;
    a->radius = ( rnd() * 2.0 * g.ship.radius ) - ( rnd() * 0.5 * g.ship.radius  );
    a->textureId = rand() % 6;
    //cout << "Created asteroid with Texture ID: " << a->textureId << endl;
    //cout << a->radius << " a radius   ship radius " << g->ship.radius << endl;

    Flt r2 = a->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = ( PI * 2.0 ) / (Flt)a->nverts;

    for ( int i=0; i<a->nverts; i++ )
    {

        a->vert[i][0] = sin( angle ) * ( r2 + rnd() * a->radius );
        a->vert[i][1] = cos( angle ) * ( r2 + rnd() * a->radius );
        angle += inc;

    }

    a->pos[0] = (Flt)( rand() % xres );
    a->pos[1] = (Flt)( rand() % yres );
    a->pos[2] = 0.0f;
    a->angle = 0.0;
    a->rotate = rnd() * 4.0 - 2.0;

    Flt tmpx = xres >> 2;
    Flt tmpy = yres >> 2;

    if ( a->pos[0] < ( 3.0 * tmpx ) &&
            a->pos[0] > tmpx &&
            a->pos[1] < ( 3.0 * tmpy ) &&
            a->pos[1] > tmpy
       ) //protect area near center from bigger asteroids
    {

        <<<<<<< HEAD
            if ( a->pos[0] < ( 2.0 * tmpx ) &&
                    a->pos[0] > tmpx
               ) //left half
            {
                a->pos[0] -= tmpx;
                a->pos[2] = 0.0f;
            }

            else //right half
            {
                a->pos[0] += tmpx;
                a->pos[2] = 0.0f;
            }
        =======
            if ( a->pos[0] < ( 2.0 * tmpx ) &&
                    a->pos[0] > tmpx
               ) //left half
            {

                a->pos[0] -= tmpx;
                a->pos[2] = 0.0f;

            }

            else //right half
            {

                a->pos[0] += tmpx;
                a->pos[2] = 0.0f;

            }
        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

    }

    if ( a->radius < g.ship.radius )
    {

        a->color[0] = 0.9;
        a->color[1] = 0.6;
        a->color[2] = 0.3;

    }

    else
    {
        <<<<<<< HEAD
            a->color[0] = 0.3;
        =======

            a->color[0] = 0.3;
        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
            a->color[1] = 0.4;
        a->color[2] = 0.5;

    }

    a->vel[0] = (Flt)( rnd() * 2.0 - 1.0 );
    a->vel[1] = (Flt)( rnd() * 2.0 - 1.0 );

    asteroidRadiusSpeed(a);

    //add to front of linked list

    a->next = g.ahead;

    if (g.ahead != NULL)
    {
        <<<<<<< HEAD
            g.ahead->prev = a;
        =======

            g.ahead->prev = a;

        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
    }

    g.ahead = a;
    g.nasteroids++;

}


void deleteAsteroid( Asteroid *node )
{

    g.nasteroids--;
    //remove a node from linked list

    if ( !g.done )
    {

        if ( node )
        {

            if ( node->prev == NULL )
            {

                <<<<<<< HEAD
                    if ( node->next == NULL )
                    {
                        g.ahead = NULL;
                    }

                    else
                    {
                        node->next->prev = NULL;
                        g.ahead = node->next;
                    }
                =======
                    if ( node->next == NULL )
                    {

                        g.ahead = NULL;

                    }

                    else
                    {

                        node->next->prev = NULL;
                        g.ahead = node->next;

                    }
                >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

            }

            else
            {

                <<<<<<< HEAD
                    if ( node->next == NULL )
                    {
                        node->prev->next = NULL;
                    }

                    else
                    {
                        node->prev->next = node->next;
                        node->next->prev = node->prev;
                    }
                =======
                    if ( node->next == NULL )
                    {

                        node->prev->next = NULL;

                    }

                    else
                    {

                        node->prev->next = node->next;
                        node->next->prev = node->prev;

                    }
                >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

            }

            delete node;
            node = NULL;

            <<<<<<< HEAD
        }
        =======
    }

    >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
}

}


void asteroidRadiusSpeed( Asteroid *a )
{   

    cout << "asteroidRadiusSpeed ftn ------------------------\n";
    cout << "axVel: " << a->vel[0] << endl;
    cout << "ayVel: " << a->vel[1] << endl;

    cout << a->radius << ": asteroid radius\n";


    Flt radiusVel = ( ( 500.0 / ( a->radius + 100.0 ) ) - 2.0 );

    if ( radiusVel < 0 )
    {
        <<<<<<< HEAD
            radiusVel = 0.05;
        =======

            radiusVel = 0.05;

        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
    }

    Flt xVel = a->vel[0];
    Flt yVel = a->vel[1];
    Flt speed = sqrt( ( xVel * xVel ) + ( yVel * yVel ) );

    cout << "initial speed: " << speed << endl;

    Flt xRatio = ( xVel / speed ) * radiusVel;
    Flt yRatio = ( yVel / speed ) * radiusVel;

    a->vel[0] = xRatio;
    a->vel[1] = yRatio;

    cout << "asteroidRadiusSpeed ftn ##### AFTER Radius Change !!!!!!!!!!!!!!!!!\n";
    cout << "axVel: " << a->vel[0] << endl;
    cout << "ayVel: " << a->vel[1] << endl;
    cout << "adjusted speed: " 
        << sqrt( ( a->vel[0] * a->vel[0] ) + ( a->vel[1] * a->vel[1] ) ) 
        << endl;

}


void shipRadiusSpeed()
{   

    cout << "shipRadiusSpeed ftn &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
    cout << g.ship.vel[0] << " ShipXVel\n";
    cout << g.ship.vel[1] << " ShipYVel\n";

    cout << g.ship.radius << ": ship radius\n";


    Flt radiusVel = ( ( 500.0 / ( g.ship.radius + 100.0 ) ) - 2.0 );

    if ( radiusVel < 0 )
    {
        <<<<<<< HEAD
            radiusVel = 0.01;
        =======

            radiusVel = 0.01;

        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
    }

    Flt xVel = g.ship.vel[0];
    Flt yVel = g.ship.vel[1];
    Flt speed = sqrt( ( xVel * xVel ) + ( yVel * yVel ) );

    cout << "initial speed: " << speed << endl;

    Flt xRatio = ( xVel / speed ) * radiusVel;
    Flt yRatio = ( yVel / speed ) * radiusVel;

    g.ship.vel[0] = xRatio;
    g.ship.vel[1] = yRatio;

    cout << "shipRadiusSpeed ftn &&&&&&&&&&& AFTER CHANGE &&&&&&&&&&&&&&&\n";
    cout << g.ship.vel[0] << " ShipXVel\n";
    cout << g.ship.vel[1] << " ShipYVel\n";
    cout << "adjusted speed: " 
        << sqrt( ( g.ship.vel[0] * g.ship.vel[0] ) + ( g.ship.vel[1] * g.ship.vel[1] ) ) 
        << endl;

}


int smithCheckKeys( XEvent *e )
{

    static int shift=0;
    int key = XLookupKeysym(&e->xkey, 0);

    if (e->type == KeyRelease)
    {

        <<<<<<< HEAD
            keys[key]=0;
        if (key == XK_Shift_L || 
                key == XK_Shift_R)
        {
            shift=0;
        }
        =======
            keys[key]=0;

        if (key == XK_Shift_L || 
                key == XK_Shift_R)
        {

            shift=0;

        }
        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

            return 0;

    }

    if (e->type == KeyPress)
    {
        <<<<<<< HEAD
            keys[key]=1;

        if (key == XK_Shift_L || 
                key == XK_Shift_R) 
        {
            shift=1;
            return 0;
        }
        =======

            keys[key]=1;

        if (key == XK_Shift_L || 
                key == XK_Shift_R) 
        {

            shift=1;
            return 0;

        }
        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

    }

    else
    {
        <<<<<<< HEAD
            return 0;
        =======

            return 0;

        >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd
    }

    if (shift)
    {

    }

    switch(key)
    {

        <<<<<<< HEAD
        case XK_r:
            if ( g.gameOver )
            {
                remakeGame();	
            }

            break;
            =======
        case XK_r:

                if ( g.gameOver )
                {

                    remakeGame();	

                }
                >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

    }

    return 0;

}


void deleteAllAsteroid()
{

    <<<<<<< HEAD
        while ( g.ahead != NULL )
        {
            Asteroid *temp = g.ahead->next;
            delete g.ahead;
            g.ahead = temp;
            g.nasteroids--;

            if (g.ahead == NULL)
            {
                break;
            }

        }

    if ( g.nasteroids == 0 )
    {
        cout << "all asteroids have been properly deleted\n";
    }
    =======
        while ( g.ahead != NULL )
        {

            Asteroid *temp = g.ahead->next;
            delete g.ahead;
            g.ahead = temp;
            g.nasteroids--;

            if (g.ahead == NULL)
            {

                break;

            }

        }

    if ( g.nasteroids == 0 )
    {

        cout << "all asteroids have been properly deleted\n";

    }
    >>>>>>> ede3c50ea9add47e53f216280f28e630850401bd

}


void remakeGame()
{

    g.score = 0.0;
    g.ship.pos[0] = (Flt) ( xres / 2 );
    g.ship.pos[1] = (Flt) ( yres / 2 );
    g.ship.radius = 20.0;
    g.gameOver = 0;
    deleteAllAsteroid();
    init();

}
