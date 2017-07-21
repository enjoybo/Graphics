#ifndef HEADER_SPLINE
#define HEADER_SPLINE

#include "hw3.h"
#include "mathclass.h"
#include <vector>
#include <list>
#include <math.h>
using namespace std;
using namespace jhm;

const int degree = 2;

class Curve{
	private:
		SplineType splineType;
		void generateCurvePoints();
		void generateCurvePointT(int nth, double t, double M[4][4]);

	public:


		std::vector<point2> CPList;
		double		scale;
		quater		rot_quat;
		position	translate;

		Curve();
		Curve(SplineType type, double scf, quater rot, position trans);
		Curve(SplineType type, std::vector<point2>& cplist, double scf, quater rot, position trans);

		std::vector<position> curvePoints;
		SplineType getSplineType();

		void setCurvePoints(std::vector<position>& pointList);
		std::vector<position> getCurvePoints();

		
		
		void drawCurve();
		void transform();
};

class Spline{
	private:
		SplineType splineType;
		std::vector<curveInfo> splineInfo;	
		//std::vector<Curve> baseCurves;
		std::vector<Curve> splineCurves;
		void generateCurves();

		double interpolateScale(int nth, double t);
		quater interpolateRot(int nth, double t);
		position interpolateTrans(int nth, double t);
		position interpolatePos(position p0, position p1, position p2, position p3, double t);
		std::vector<position> interpolatePoints(int nth, double t);
		Curve interpolateCurvesT(int nth, double t);

		void interpolateCurves();
	public:
		std::vector<Curve> baseCurves;//

		Spline();
		Spline(SplineType type, std::vector<curveInfo>& info);

		void drawSpline();
};




#endif
