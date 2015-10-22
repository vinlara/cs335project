/*  Eric Smith
    ericS.cpp

	CS 335 Software Engineering Project

	Group Members:
	Abdulelah Aldeshash
	Eric Smith
	Erik Juarez
	Perry Huynh
	Vincente Lara
*/
#include <cstdlib>

typedef float Flt;
typedef float Vec[3];
typedef Flt Matrix[4][4];

#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0

struct Ship {
    Vec dir;
    Vec pos;
    Vec vel;
    float angle;
    Flt radius;
    float color[3];
    Ship() {
        VecZero(dir);
        pos[0] = (Flt)(1280/2);
        pos[1] = (Flt)(960/2);
        pos[2] = 0.0f;
        VecZero(vel);
        angle = 0.0;
        color[0] = 1.0;
        color[1] = 1.0;
        color[2] = 1.0;
    }
};

struct Asteroid {
    Vec pos;
    Vec vel;
    int nverts;
    Flt radius;
    Vec vert[8];
    float angle;
    float rotate;
    float color[3];
    struct Asteroid *prev;
    struct Asteroid *next;
    Asteroid() {
        prev = NULL;
        next = NULL;
    }
};

struct Game {
    Ship ship;
    Asteroid *ahead;
    int nasteroids;
    struct timespec bulletTimer;
    Game() {
        ahead = NULL;
        nasteroids = 0;
    }
};


void deleteAsteroid(Game *g, Asteroid *node)
{
	//remove a node from linked list
	if (g)
	{
		if (node)
		{
			if (node->prev == NULL)
			{
				if (node->next == NULL)
				{
					g->ahead = NULL;
				}
				else
				{
					node->next->prev = NULL;
					g->ahead = node->next;
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

