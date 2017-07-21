#ifndef HEADER_SURFACE
#define HEADER_SURFACE

#include "mathclass.h"
#include <vector>

using namespace std;
using namespace jhm;

class Segment{
	public:
		jhm::vector norm;
		jhm::vector p0, p1;

		Segment() {};
		Segment(jhm::vector n, jhm::vector p0, jhm::vector p1);
};

class Surface{
	public:
		jhm::vector normal;
		jhm::vector p0, p1, p2, p3;
		std::vector<Segment> segments;
		jhm::vector color;
		double alpha;
		
		Surface() {};
		Surface(jhm::vector n, jhm::vector p0, jhm::vector p1, jhm::vector p2, jhm::vector p3, jhm::vector clr, double a);
		bool operator < (const Surface& that) const;
};

#endif
