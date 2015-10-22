//
// Perry Huynh
// Lab 6
//
#include <cmath>

typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

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
	v[0] *= len * 3;
	v[1] *= len * 3;
}
