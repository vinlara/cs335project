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

