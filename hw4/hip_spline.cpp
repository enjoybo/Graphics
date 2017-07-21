#include "spline.h"
#include <iostream>
using namespace std;
const int degree = 4;

double Bezier_Matrix[4][4] = {{ -1,  3, -3,  1},
							  {  3, -6,  3,  0},
							  { -3,  3,  0,  0}, 
							  {  1,  0,  0,  0}};

double Bspline_Matrix[4][4] = {{ -1/6.0,  3/6.0, -3/6.0,  1/6.0},
							   {  3/6.0, -6/6.0,  3/6.0,  0/6.0},
							   { -3/6.0,  0/6.0,  3/6.0,  0/6.0}, 
							   {  1/6.0,  4/6.0,  1/6.0,  0/6.0}};


const double tng = 4.0;
double Catmull_Rom_Matrix[4][4] = {{-3/tng, 2-3/tng, 3/tng-2,  3/tng},
								   { 6/tng, 3/tng-3, 3-6/tng, -3/tng},
								   {-3/tng,		  0,   3/tng,	   0}, 
								   {     0,       1,       0,        0}};

// class curve 
Curve::Curve(){
}

Curve::Curve(SplineType type, double scf, quater rot, position trans){
	splineType = type;
	scale = scf;
	rot_quat = rot;
	translate = trans;
}

Curve::Curve(SplineType type, std::vector<point2>& cplist, double scf, quater rot, position trans){
	splineType = type;
	CPList = cplist;
	scale = scf;
	rot_quat = rot;
	translate = trans;

	generateCurvePoints();
}

void Curve::generateCurvePoints(){
	int nCP = CPList.size();
	curvePoints.clear();

	double t = 1/(double)degree;
	for(int i=0; i<nCP; i++){
		int limit;
		if(i==nCP-1)
			limit = degree;
		else
			limit = degree-1;
		for(int j=0; j<=limit; j++){
			if(splineType == BSPLINE){
				generateCurvePointT(i, j*t, Bspline_Matrix);
			}
			else{
				generateCurvePointT(i, j*t, Catmull_Rom_Matrix);
			}
		}
	}
}

void Curve::generateCurvePointT(int nth, double t, double M[4][4]){
	int nCP = CPList.size();
	double T[] = {pow(t, 3), pow(t, 2), t, 1};
	
	point2 p0 = CPList[nth];
	point2 p1 = CPList[(nth+1)%nCP];
	point2 p2 = CPList[(nth+2)%nCP];
	point2 p3 = CPList[(nth+3)%nCP];
	point2 P[4] = {p0, p1, p2, p3};
	
	position pt(0, 0, 0);
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			pt[0] += T[i]*M[i][j]*P[j].x();
			pt[2] += T[i]*M[i][j]*P[j].y();
		}
	}
	curvePoints.push_back(pt);
}	

SplineType Curve::getSplineType(){
	return splineType;
}

void Curve::setCurvePoints(std::vector<position>& pointList){
	curvePoints = pointList;
}

std::vector<position> Curve::getCurvePoints(){
	return curvePoints;
}


void Curve::drawCurve()
{	
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0,0.0,0.0);
		for(int i=0; i<(int)curvePoints.size(); i++){
			glVertex3d(curvePoints[i].x(), 0, curvePoints[i].y());
		}
	glEnd();
}

void Curve::transform()
{
	for(int i=0; i<(int)curvePoints.size(); i++){
		curvePoints[i] *= scale;
		jhm::vector v = jhm::vector(curvePoints[i].x(), curvePoints[i].y(), curvePoints[i].z());

		if(ln(rot_quat).length()!=0){
			v = rotate(rot_quat, v);
			curvePoints[i] = position(v.x(), v.y(), v.z());
		}
		curvePoints[i] += jhm::vector(translate.x(), translate.y(), translate.z());
	}
}


// class spline
Spline::Spline(){
}

Spline::Spline(SplineType type, std::vector<curveInfo>& info){
	splineInfo = info;
	splineType = type;


	generateCurves();
	interpolateCurves();
}

void Spline::generateCurves(){
	baseCurves.clear();

	int len = splineInfo.size();
	for(int i=0; i<len; i++){
		Curve newCurve(splineType, splineInfo[i].CPList, splineInfo[i].scale, splineInfo[i].rotate, splineInfo[i].translate);
		baseCurves.push_back(newCurve);
	}
}

position interpolate(position p0, position p1, position p2, position p3, double t, double M[4][4]){
	double T[] = {pow(t, 3), pow(t, 2), t, 1};
	
	position p[4] = {p0,p1,p2,p3};
	position pt(0, 0, 0);
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			pt[0] += T[i]*M[i][j]*p[j].x();
			pt[1] += T[i]*M[i][j]*p[j].y();
			pt[2] += T[i]*M[i][j]*p[j].z();
		}
	}
	return pt;
}

void Spline::interpolateCurves(){
	int n = baseCurves.size();
	splineCurves.clear();

	int i;
	for(i=0; i<n; i++){
		baseCurves[i].transform();
	}

	double interpol_degree = 5;//degree
	double t = 1/(double)interpol_degree;
	for(i=1; i<n-2; i++){
		if(splineType == BSPLINE){
			for(int j=0; j<interpol_degree; j++){
				std::vector<position> cp;
				for(int k=0; k<(int)baseCurves[i].curvePoints.size(); k++){
					cp.push_back(interpolate(
								baseCurves[i-1].curvePoints[k],
								baseCurves[i].curvePoints[k],
								baseCurves[i+1].curvePoints[k],
								baseCurves[i+2].curvePoints[k],
							j*t, Bspline_Matrix));
				}
				Curve curve;
				curve.setCurvePoints(cp);
				splineCurves.push_back(curve);
			}
		}
		else{
			for(int j=0; j<interpol_degree; j++){
				std::vector<position> cp;
				for(int k=0; k<(int)baseCurves[i].curvePoints.size(); k++){
					cp.push_back(interpolate(
								baseCurves[i-1].curvePoints[k],
								baseCurves[i].curvePoints[k],
								baseCurves[i+1].curvePoints[k],
								baseCurves[i+2].curvePoints[k],
							j*t, Catmull_Rom_Matrix));
				}
				Curve curve;
				curve.setCurvePoints(cp);
				splineCurves.push_back(curve);
			}
		}
	}
	splineCurves.push_back(baseCurves[i+1]);
}

jhm::vector getAvgNorm(position po, position pr, position pu, position pl, position pd){
	jhm::vector left, right, up, down, vN1, vN2, vN3, vN4;
	left = pl - po;
	right = pr - po;
	up = pu - po;
	down = pd - po;

	vN1 = right*up;
	vN2 = up*left;
	vN3 = left*down;
	vN4 = down*right;

	return -(vN1 + vN2 + vN3 + vN4).normalize();
}


	
void Spline::drawSpline(){
	glColor3f(1,1,1);
	//glColor3f(0.93, 0.38, 0.5);	
	std::vector<std::vector<jhm::vector> > normalVectors;
	int s = splineCurves.size();
	
	//without interpolation
	/*for(int i=0; i<splineCurves.size()-1; i++){
		glBegin(GL_QUAD_STRIP);	

		Curve curvem1, curve0, curve1;
		if(i!=0){
			curvem1 = splineCurves[i-1];
		}
		curve0 = splineCurves[i];
		curve1 = splineCurves[i+1];


		int nPoints = curve0.curvePoints.size();
		
		for(int j=0; j<nPoints; j++){
			position p0, p1, p2;
			jhm::vector v0,v1,vN1,vN2,vN;
			p0 = curve0.curvePoints[j];
			p1 = curve1.curvePoints[j];
			p2 = curve0.curvePoints[(j+1)%nPoints];
			v0 = p0 - p1;
			v1 = p2 - p1;
			vN1 = v0 * v1;
			vN1 = vN1.normalize();
			vN = vN1;
				
			vN = vN.normalize();
			p1 = curve1.curvePoints[j];



			glNormal3d(vN.x(), vN.y(), vN.z());
			glVertex3d(p0.x(), p0.y(), p0.z());
			glVertex3d(p1.x(), p1.y(), p1.z());

		}

		glEnd();
	}*/

	for(int i=1; i<splineCurves.size()-1; i++){
		normalVectors.push_back(std::vector<jhm::vector>());
		//glBegin(GL_QUAD_STRIP);	

		Curve curvem1, curve0, curve1;
		curvem1 = splineCurves[i-1];
		curve0 = splineCurves[i];
		curve1 = splineCurves[i+1];


		int nPoints = curve0.curvePoints.size();
		
		for(int j=0; j<nPoints; j++){
			position p0, p1, p2;
			jhm::vector v0,v1,vN1,vN2,vN;
			p0 = curve0.curvePoints[j];
			p1 = curve1.curvePoints[j];
			p2 = curve0.curvePoints[(j+1)%nPoints];
							
			vN = getAvgNorm(p0, p2, curvem1.curvePoints[j], curve0.curvePoints[(j-1+nPoints)%nPoints], p1);
			normalVectors[i-1].push_back(vN);

			//glNormal3d(vN.x(), vN.y(), vN.z());
			//glVertex3d(p0.x(), p0.y(), p0.z());
			//glVertex3d(p1.x(), p1.y(), p1.z());

		}
		
		//glEnd();
	}
	
	for(int i=1; i<splineCurves.size()-2; i++){
		glBegin(GL_QUAD_STRIP);
		int nPoints0 = splineCurves[i].curvePoints.size();
		int nPoints1 = splineCurves[i+1].curvePoints.size();
		for(int j=0; j<nPoints0; j++){
			jhm::vector v1, v2, v3, v4, vx, vy, vN;
			v1 = normalVectors[i-1][j];
			v2 = normalVectors[i-1][(j+1)%nPoints0];
			v3 = normalVectors[i][j];
			v4 = normalVectors[i][(j+1)%nPoints1];

			position pO = splineCurves[i].curvePoints[j];
			vx = splineCurves[i].curvePoints[(j+1)%nPoints0] - splineCurves[i].curvePoints[j];
			vy = splineCurves[i+1].curvePoints[j] - splineCurves[i].curvePoints[j];

			double xsize = vx.length();
			double ysize = vy.length();
			const double delta = 0.1;
			
			for(double dy=0; dy<ysize; dy+=delta){
				for(double dx=0; dx<xsize; dx+=delta){
					position p0, p1;
					p0 = pO + dx*vx + dy*vy;
					p1 = pO + dx*vx + (dy+delta)*vy;

					vN = (ysize - dy)/(ysize - 0) * ( (xsize-dx)/(xsize-0)*v1 + (dx-0)/(xsize-0)*v2) 
						+ (dy-0)/(ysize-0) * ( (xsize-dx)/(xsize-0)*v3 + (dx-0)/(xsize-0)*v4);
					
					glNormal3d(vN.x(), vN.y(), vN.z());
					glVertex3d(p0.x(), p0.y(), p0.z());
					glVertex3d(p1.x(), p1.y(), p1.z());

				}
			}
		}
		glEnd();
	}

	
	
	/*for(int i=0; i<splineCurves.size()-1; i++){
		glBegin(GL_LINE_LOOP);	

		Curve curve0 = splineCurves[i];
		Curve curve1 = splineCurves[i+1];


		int nPoints = curve0.curvePoints.size();
		
		for(int j=0; j<nPoints; j++){
			position p0 = curve0.curvePoints[j];
			position p1 = curve1.curvePoints[j];
			position p2 = curve0.curvePoints[(j+1)%nPoints];
	
			jhm::vector v0 = p0 - p1;
			jhm::vector v1 = p1 - p2;
			jhm::vector vN = v0 * v1;

			glVertex3d(p0.x(), p0.y(), p0.z());


		}
		glEnd();
	}*/

}
