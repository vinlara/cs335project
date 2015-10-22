void checkMouse(XEvent *e, Game *g)
{
	if (g->ship.savex != e->xbutton.x || g->ship.savey != e->xbutton.y)
	{
		//Mouse moved
		g->ship.savex = e->xbutton.x;
		g->ship.savey = e->xbutton.y;
		int y = yres - e->xbutton.y;
		float dx = g->ship.savex - g->ship.pos[0];
		float dy = y - g->ship.pos[1];
		float len = sqrt(dx * dx + dy * dy);
		g->ship.vel[0] = dx / len;
		g->ship.vel[1] = dy / len;
		normalize(g->ship.vel);
		return;
	}
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
	v[0] *= len * 3;
	v[1] *= len * 3;
}
