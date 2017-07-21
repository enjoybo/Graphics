#include "spline.h"
#include "surface.h"
#include <iostream>
#include <algorithm>
using namespace std;
const int degree = 15;

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

	double interpol_degree = 10;//degree
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
	glColor3f(0.38,0.93,0.5);
	//glColor4f(1,1,1,0.5);
	//glColor3f(0.93, 0.38, 0.5);	
	std::vector<std::vector<jhm::vector> > normalVectors;
	std::vector<Surface> opaqueSurfaces;
	std::vector<Surface> translucentSurfaces;
	int s = splineCurves.size();

	//calculataing normal vector for each vertex
	//0th curve
	normalVectors.push_back(std::vector<jhm::vector>());
	for(int j=0; j<splineCurves[0].curvePoints.size(); j++){
		int nSize = splineCurves[0].curvePoints.size();
		position p0, p1, p2, p3;
		jhm::vector v0, v1, v2, vN1, vN2, vN;
		p0 = splineCurves[0].curvePoints[j];
		p1 = splineCurves[1].curvePoints[j];
		p2 = splineCurves[0].curvePoints[(j+1)%nSize];
		p3 = splineCurves[0].curvePoints[(j-1+nSize)%nSize];
		v0 = p2 - p0;
		v1 = p1 - p0;
		v2 = p3 - p0;
		vN1 = v0 * v1;
		vN2 = v1 * v2;
		vN = (vN1+vN2).normalize();
		normalVectors[0].push_back(vN);	
	}
	//1st to (n-2)th curves
	for(int i=1; i<splineCurves.size()-1; i++){
		normalVectors.push_back(std::vector<jhm::vector>());

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
			normalVectors[i].push_back(vN);

		}
	}
	//(n-1)th curve
	normalVectors.push_back(std::vector<jhm::vector>());
	for(int j=0; j<splineCurves[splineCurves.size()-1].curvePoints.size(); j++){
		int nSize = splineCurves[splineCurves.size()-1].curvePoints.size();
		position p0, p1, p2, p3;
		jhm::vector v0, v1, v2, vN1, vN2, vN;
		p0 = splineCurves[splineCurves.size()-1].curvePoints[j];
		p1 = splineCurves[splineCurves.size()-2].curvePoints[j];
		p2 = splineCurves[splineCurves.size()-1].curvePoints[(j+1)%nSize];
		p3 = splineCurves[splineCurves.size()-1].curvePoints[(j-1+nSize)%nSize];
		v0 = p2 - p0;
		v1 = p1 - p0;
		v2 = p3 - p0;
		vN1 = v1 * v0;
		vN2 = v2 * v1;
		vN = (vN1+vN2).normalize();
		normalVectors[splineCurves.size()-1].push_back(vN);	
	}

	//making (opaque) spline into surfaces
	for(int i=0; i<splineCurves.size()-1; i++){
		int nPoints0 = splineCurves[i].curvePoints.size();
		int nPoints1 = splineCurves[i+1].curvePoints.size();
		for(int j=0; j<nPoints0; j++){
			position p0, p1, p2;
			jhm::vector s0, s1, sN;
			
			p0 = splineCurves[i].curvePoints[j];
			p1 = splineCurves[i+1].curvePoints[j];
			p2 = splineCurves[i].curvePoints[(j+1)%nPoints0];
			s0 = p2 - p0;
			s1 = p1 - p0;
			sN = s1 * s0;
			sN = sN.normalize();

			jhm::vector v0, v1, v2, v3, vx, vy, vN;
			v0 = normalVectors[i][j];
			v1 = normalVectors[i][(j+1)%nPoints0];
			v2 = normalVectors[i+1][j];
			v3 = normalVectors[i+1][(j+1)%nPoints1];
			
			jhm::vector pO = splineCurves[i].curvePoints[j] - position(0,0,0);
			jhm::vector pL = splineCurves[i+1].curvePoints[(j+1)%nPoints1] - position(0,0,0);
			vx = splineCurves[i].curvePoints[(j+1)%nPoints0] - splineCurves[i].curvePoints[j];
			vy = splineCurves[i+1].curvePoints[j] - splineCurves[i].curvePoints[j];

			Surface surface(sN, pO, pO+vx, pO+vy, pL, jhm::vector(1,1,1), 1.0);

			double xsize = vx.length();
			double ysize = vy.length();

			const double normal_degree = 10;//
			const double delta = 1.0/normal_degree;
			
			for(int k=0; k<normal_degree; k++){
				for(int l=0; l<=normal_degree; l++){
					double dy = k*delta*ysize, dx = l*delta*xsize;
					jhm::vector p0, p1;
					p0 = (ysize - dy)/(ysize - 0) * ( (xsize-dx)/(xsize-0)* pO + (dx-0)/(xsize-0)*(pO+vx)) 
						+ (dy-0)/(ysize-0) * ( (xsize-dx)/(xsize-0)*(pO+vy) + (dx-0)/(xsize-0)*pL);
					dy = (k+1)*delta*ysize;
					p1 = (ysize - dy)/(ysize - 0) * ( (xsize-dx)/(xsize-0)* pO + (dx-0)/(xsize-0)*(pO+vx)) 
						+ (dy-0)/(ysize-0) * ( (xsize-dx)/(xsize-0)*(pO+vy) + (dx-0)/(xsize-0)*pL);
					dy = k*delta*ysize;
					vN = (ysize - dy)/(ysize - 0) * ( (xsize-dx)/(xsize-0)*v0 + (dx-0)/(xsize-0)*v1) 
						+ (dy-0)/(ysize-0) * ( (xsize-dx)/(xsize-0)*v2 + (dx-0)/(xsize-0)*v3);
					
					Segment segment(vN, p0, p1);
					surface.segments.push_back(segment);

				}
			}
			opaqueSurfaces.push_back(surface);
		}
	}

	//making cube into surfaces
	jhm::vector left(1, 0, 0), up(0, 1, 0), front(0, 0, 1);
	jhm::vector orig0(0, 4, -1);
	double size0 = 1.5;
	jhm::vector color0(0.38, 0.93, 0.5);
	double alpha0 = 0.5;
	jhm::vector norm[6]={left,-left,up,-up,front,-front}, vecx[6]={up, -up, front, -front, left, -left}, vecy[6]={front, -front, left, -left, up, -up};
	for(int i=0; i<6; i++){
		Surface surface(norm[i],
				orig0 + norm[i]*size0/2 - vecx[i]*size0/2 - vecy[i]*size0/2, 
				orig0 + norm[i]*size0/2 + vecx[i]*size0/2 - vecy[i]*size0/2, 
				orig0 + norm[i]*size0/2 - vecx[i]*size0/2 + vecy[i]*size0/2, 
				orig0 + norm[i]*size0/2 + vecx[i]*size0/2 + vecy[i]*size0/2, 
				color0, alpha0);
		Segment segment0(norm[i], orig0 + norm[i]*size0/2 - vecx[i]*size0/2 - vecy[i]*size0/2, orig0 + norm[i]*size0/2 + vecx[i]*size0/2 - vecy[i]*size0/2);
		surface.segments.push_back(segment0);
		Segment segment1(norm[i], orig0 + norm[i]*size0/2 - vecx[i]*size0/2 + vecy[i]*size0/2, orig0 + norm[i]*size0/2 + vecx[i]*size0/2 + vecy[i]*size0/2);
		surface.segments.push_back(segment1);
		translucentSurfaces.push_back(surface);
	}

	/*jhm::vector orig1(1, 5.5, -2.5);
	double size1 = 1.5;
	jhm::vector color1(0.38, 0.93, 0.5);
	double alpha1 = 0.5;
	for(int i=0; i<6; i++){
		Surface surface(norm[i],
				orig1 + norm[i]*size1/2 - vecx[i]*size1/2 - vecy[i]*size1/2, 
				orig1 + norm[i]*size1/2 + vecx[i]*size1/2 - vecy[i]*size1/2, 
				orig1 + norm[i]*size1/2 - vecx[i]*size1/2 + vecy[i]*size1/2, 
				orig1 + norm[i]*size1/2 + vecx[i]*size1/2 + vecy[i]*size1/2, 
				color1, alpha1);
		Segment segment0(norm[i], orig1 + norm[i]*size1/2 - vecx[i]*size1/2 - vecy[i]*size1/2, orig1 + norm[i]*size1/2 + vecx[i]*size1/2 - vecy[i]*size1/2);
		surface.segments.push_back(segment0);
		Segment segment1(norm[i], orig1 + norm[i]*size1/2 - vecx[i]*size1/2 + vecy[i]*size1/2, orig1 + norm[i]*size1/2 + vecx[i]*size1/2 + vecy[i]*size1/2);
		surface.segments.push_back(segment1);
		translucentSurfaces.push_back(surface);
	}*/

	
	//sort translucent surfaces
	std::sort(translucentSurfaces.begin(), translucentSurfaces.end());

	//draw opque surfaces first
	for(int i=0; i<(int)opaqueSurfaces.size(); i++){
		glBegin(GL_QUAD_STRIP);
		jhm::vector clr = opaqueSurfaces[i].color;
		glColor4f(clr.x(), clr.y(), clr.z(), opaqueSurfaces[i].alpha);
		for(int j=0; j<(int)opaqueSurfaces[i].segments.size(); j++){
			jhm::vector vN, p0, p1;
			vN = opaqueSurfaces[i].segments[j].norm;
			p0 = opaqueSurfaces[i].segments[j].p0;
			p1 = opaqueSurfaces[i].segments[j].p1;
			glNormal3d(vN.x(), vN.y(), vN.z());
			glVertex3d(p0.x(), p0.y(), p0.z());
			glVertex3d(p1.x(), p1.y(), p1.z());
		}
		glEnd();
	}

	//draw translucent surfaces
	//material from http://devernay.free.fr/cours/opengl/materials.html
	GLfloat amb[6][4] = {
		{0.24725, 0.1995, 0.0745, 0.5}, //gold
		{0.0, 0.0, 0.0, 0.5}, //red plastic
		{0.0, 0.05, 0.0, 0.5}, //green rubber
		{0.0215, 0.1745, 0.0215, 0.5}, //emerald
		{0.25, 0.20725, 0.20725, 0.5}, //pearl
		{0.329412, 0.223529, 0.027451, 0.5} //brass
	};
	GLfloat dif[6][4] = {
		{0.75164, 0.60648, 0.22648, 0.5}, //gold
		{0.5, 0.0, 0.0, 0.5}, //red plastic
		{0.4, 0.5, 0.4, 0.5}, //green rubber
		{0.07568, 0.61424, 0.07568, 0.5}, //emerald
		{1, 0.829, 0.829, 0.5}, //pearl
		{0.780392, 0.568627, 0.113725, 0.5} //brass
	};
	GLfloat spec[6][4] = {
		{0.628281, 0.555802, 0.366065, 0.5}, //gold
		{0.7, 0.6, 0.6, 0.5}, //red plastic
		{0.04, 0.7, 0.04, 0.5}, //green rubber
		{0.633, 0.727811, 0.633, 0.5}, //emerald
		{0.296648, 0.296648, 0.296648, 0.5}, //pearl
		{0.992157, 0.941176, 0.807843, 0.5} //brass
	};
	GLfloat shine[6] = {
		0.4, //gold
		0.25, //red plastic
		0.078125, //green rubber
		0.6, //emerald
		0.088, //pearl
		0.21794872 //brass
	};



	for(int i=0; i<(int)translucentSurfaces.size(); i++){
		glBegin(GL_QUAD_STRIP);
		jhm::vector clr = translucentSurfaces[i].color;
		//
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec[i]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine[i]*128);
		
		glColor4f(clr.x(), clr.y(), clr.z(), translucentSurfaces[i].alpha);
		//glColor3f(clr.x(), clr.y(), clr.z());


		for(int j=0; j<(int)translucentSurfaces[i].segments.size(); j++){
			jhm::vector vN, p0, p1;
			vN = translucentSurfaces[i].segments[j].norm;
			p0 = translucentSurfaces[i].segments[j].p0;
			p1 = translucentSurfaces[i].segments[j].p1;
			glNormal3d(vN.x(), vN.y(), vN.z());
			glVertex3d(p0.x(), p0.y(), p0.z());
			glVertex3d(p1.x(), p1.y(), p1.z());
		}
		glEnd();
	}

	//alpha changed cube for contrast
	glColor4f(0.38, 0.93, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(0.0, 4.0, 1.0);
	glScalef(1.5, 1.5, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();



	
	
	//draw curves
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
