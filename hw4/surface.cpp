#include "surface.h"

extern jhm::vector getCameraVec();

Segment::Segment(jhm::vector n, jhm::vector p0, jhm::vector p1){
	norm = n;
	this->p0 = p0;
	this->p1 = p1;
}

Surface::Surface(jhm::vector n, jhm::vector p0, jhm::vector p1, jhm::vector p2, jhm::vector p3, jhm::vector clr, double a ){
	normal = n;
	this->p0 = p0;
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	color = clr;
	alpha = a;
}


bool Surface::operator < (const Surface& that) const {
	jhm::vector camVec = -getCameraVec();
	jhm::vector norm0 = this->normal;
	jhm::vector norm1 = that.normal;
	const double eps = 0.0001;
	if(norm0%camVec < 0)
		norm0 = -norm0;
	if(norm1%camVec < 0)
		norm1 = -norm1;

	if(norm0.length() <= eps || norm1.length() <= eps)
		return this->alpha > that.alpha;


	double d0, d1;
	d0 = -norm0%this->p0;
	d1 = -norm1%that.p0;

	int retTrue = -1, retFalse = -1;
	
	if(norm0%that.p0+d0 >= 0
		&& norm0%that.p1+d0 >=0
		&& norm0%that.p2+d0 >=0
		&& norm0%that.p3+d0 >=0){
		retTrue = 1;
	}

	if(norm0%that.p0+d0 < 0
		&& norm0%that.p1+d0 < 0
		&& norm0%that.p2+d0 < 0
		&& norm0%that.p3+d0 < 0){
		retFalse = 1;
	}

	if(norm1%this->p0+d1 > 0
		&& norm1%this->p1+d1 > 0
		&& norm1%this->p2+d1 > 0
		&& norm1%this->p3+d1 > 0){
		retFalse = 1;
	}

	if(norm1%this->p0+d1 <= 0
		&& norm1%this->p1+d1 <= 0
		&& norm1%this->p2+d1 <= 0
		&& norm1%this->p3+d1 <= 0){
		retTrue = 1;
	}

	if(retTrue > 0 && retFalse < 0)
		return true;
	if(retTrue < 0 && retFalse > 0)
		return false;
	return this->alpha > that.alpha;
}
