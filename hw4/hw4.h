#ifndef HEADER_HW3
#define HEADER_HW3

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <vector>
#include "mathclass.h"
using namespace std;
using namespace jhm;

enum SplineType{BSPLINE, CATMULL_ROM};
struct curveInfo{
	std::vector<point2> CPList;
	double		scale;
	quater		rotate;
	position	translate;
};




#endif
