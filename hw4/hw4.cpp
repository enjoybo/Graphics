#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include "hw4.h"
#include "spline.h"

using namespace std;
using namespace jhm;

SplineType SPLINETYPE;
std::vector<curveInfo> splineInfo;

//
jhm::vector getCameraPos();

static float radius = 5;

position camOrigin(0, 0, 0);				// focus point of camera
position camPosition(0, 0, -10);		// default camera position vector
										// (from cuurent camera position to focus.)

jhm::vector camUp(0, 1, 0);				// current camera up vector
jhm::vector camRight(1, 0, 0);

double dist = 10.0;						// camera distance
double zoom = 90.0;						// zoom angle(FOV)

position currOri(0, 0, 0);
position nextOri(0, 0, 0);
jhm::vector rotVec(0.0, 0.0, 1.0);
double rotAngle = 0.0;

// temp vectors to watch matching points
position matchT1(0, 0, 0);
position matchT2(0, 0, 0);

bool isZoom = false;
bool isRotate = true;

jhm::vector getBaseVector(jhm::vector proj);
jhm::vector getRotVector(jhm::vector v0, jhm::vector v1);
double getRotAngle(jhm::vector v0, jhm::vector v1);
//

jhm::vector getCameraVec(){
	return camOrigin - camPosition;
}


void getTextInput();

void init(){
	glClearColor(0.1, 0.1, 0.1, 1.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);//
}

void displaySpline(){
	Spline spline(SPLINETYPE, splineInfo);

	spline.drawSpline();
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
//	gluLookAt(0.0, 0.0, -100.0, 
//			0.0, 0.0, 0.0, 
//			0.0, 1.0, 0.0);

	gluLookAt(camPosition.x(), camPosition.y(), camPosition.z(), 
			camOrigin.x(), camOrigin.y(), camOrigin.z(), 
			camUp.x(), camUp.y(), camUp.z());
	
	glLineWidth(1.0f);

	glPushMatrix();
	glColor3d(0.2, 0.2 ,0.9);
	// Creating Light Reference : http://www.falloutsoftware.com/tutorials/gl/gl8.htm
	// Create light components
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specularLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat position0[] = { 10.0f, 15.0f, -50.0f, 0.0f };
	//GLfloat position1[] = { 2.0f, -20.0f, 50.0f, 0.0f };
	GLfloat position2[] = { 0.0f, 50.0f, 80.0f, 0.0f };


	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);

	/*glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);*/

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT2, GL_POSITION, position2);


	
	GLfloat mat_specular[] = { -1.0, -1.0, -1.0, 1.0 };
	GLfloat mat_shininess[] = { 0.0 };
	glShadeModel (GL_SMOOTH);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	displaySpline();
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(zoom, 1, 5, 500);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

double getSqrt(double f){
	if(f<0) return -1;
	else return sqrt(f);
}

jhm::vector getBaseVector(jhm::vector proj){
	// t*mouseOri poses on the sphere
	position C = camPosition; //camera is now at this postion
	position F = camOrigin;
	jhm::vector P = proj; //vector from camera postion toward the mouse world coordinate

	double a = P%P, b = P%(C-F), c = (C-F)%(C-F) - radius*radius;
	double rootVal = getSqrt(b*b - a*c);

	// mouse out of trackball
	if(rootVal < 0) return jhm::vector(0,0,0);

	double t1 = (-b + rootVal)/a;
	double t2 = (-b - rootVal)/a;
	
	//world coordinate of intersectin on the trackball sphere
	// didn't consider which t to use yet
	matchT1 = C + t1*proj;
	matchT2 = C + t2*proj;	

	//returns the vector from focus(the center of the sphere) to matchT2
	return matchT2 - camOrigin;
}

// needs modification
double getRotAngle(jhm::vector v0, jhm::vector v1){

	jhm::vector cross = v0*v1;
	double dot = v0%v1;

	double norm0 = v0.length();
	double norm1 = v1.length();

	double cos = dot/(norm0*norm1);
	double sin = cross.length()/(norm0*norm1);

	float angle = atan2(sin, cos);
	return angle;
}
//

jhm::vector getRotVector(jhm::vector v0, jhm::vector v1){
	jhm::vector cross = v0*v1;
	jhm::vector res = cross.normalize() * 0.5 * getRotAngle(v0, v1);

	return res;
}

jhm::vector getMousePos(int x, int y){
	double ratio = 400/((dist-radius)*tan(zoom/2 /180.0 * 3.14159265));
	return jhm::vector(((float)x - 400)/ratio, -((float)y - 400)/ratio, 0);
}

jhm::vector getPlane(){
	return (camPosition - camOrigin).normalize() * radius;
}

void mouseMove(int x, int y){	
	jhm::vector mousePos = getMousePos(x, y);	
	nextOri = camOrigin + getPlane() - mousePos.x()*camRight + mousePos.y()*camUp;

	if (isRotate) {
		jhm::vector v0 = getBaseVector(currOri - camPosition);
		jhm::vector v1 = getBaseVector(nextOri - camPosition);
		
		if (v0.length()!=0 && v1.length()!=0) {
			jhm::vector V = camPosition - camOrigin;
			V = rotate(exp(getRotVector(v1, v0)), V);
			camPosition = camOrigin + V;
			
			camRight = rotate(exp(getRotVector(v1, v0)), camRight);
			camUp = rotate(exp(getRotVector(v1, v0)), camUp);
		}
	}
	else {
		camOrigin += currOri - nextOri;
		camPosition += currOri - nextOri;
	}

	
	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y){
	double zoomDelta = 0.1;
	double dollyDelta = 0.1;

	if (button == GLUT_LEFT_BUTTON) {
		// when the button is released
		if (state == GLUT_UP) {
		}
		else  {// state = GLUT_DOWN
			jhm::vector mousePos = getMousePos(x, y);
			currOri = camOrigin + getPlane() - mousePos.x()*camRight + mousePos.y()*camUp;
		}
	}
	else if (button == 3) {
		if (isZoom) {
			if (zoom > 0 +  zoomDelta)
				zoom -= zoomDelta;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(zoom, 1, 5, 50);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

		}
		else {
			jhm::vector V = camPosition - camOrigin;
			V = V.normalize() * (V.length() - dollyDelta);
			camPosition = camOrigin + V;
			dist -= dollyDelta;
		}
	}

	else if (button == 4) {
		if (isZoom) {
			if(zoom < 180 - zoomDelta)
				zoom += zoomDelta;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(zoom, 1, 5, 50);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();


		}
		else {
			jhm::vector V = camPosition - camOrigin;
			V = V.normalize() * (V.length() + dollyDelta);
			camPosition = camOrigin + V;
			dist += dollyDelta;

		}
	}

	glutPostRedisplay();

}

void keyboardDown(unsigned char key, int x, int y)
{
	if (key == 'z')
		isZoom = true;
	else if (key == 't')
		isRotate = false;
}

void keyboardUp(unsigned char key, int x, int y)
{
	if (key == 'z')
		isZoom = false;
	else if (key == 't')
		isRotate = true;
}


int main(int argc, char **argv){
	getTextInput();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(400, 400);

	glutCreateWindow("Spline");
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseButton);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	
	glutMainLoop();

	return 0;
}

void getTextInput(){
	string line;
	string splineType;
	
	ifstream data;
	data.open("data.txt");

	data >> splineType;
	
	if(splineType.compare("BSPLINE")==0){
		SPLINETYPE = BSPLINE;
	}
	else if(splineType.compare("CATMULL_ROM")==0){
		SPLINETYPE = CATMULL_ROM;
	}
	else{
		cout << "wrong spline type" << endl;
		exit(1);
	}

	int crossSections, controlPoints;
	data >> crossSections;
	data >> controlPoints;

	for(int i=0; i<crossSections; i++){
		curveInfo curve;
		std::vector<point2> cpL;
		for(int j=0; j<controlPoints; j++){
			double cpx, cpy;
			data >> cpx >> cpy;

			point2 cp(cpx, cpy);
			cpL.push_back(cp);
		}
		
		double scf;
		double* rot = new double[4];	
		double* pos = new double[3];

		data >> scf;

		double theta;
		jhm::vector axis;
		data >> theta >> axis[0] >> axis[1] >> axis[2];
		data >> pos[0] >> pos[1] >> pos[2];
	
		theta /= 2.0;
		if(theta < 0) {
			theta = -theta;
			axis = -axis;
		}

		curve.CPList = cpL;
		curve.scale = scf;
		if(axis.length() == 0)
			curve.rotate = quater(0,0,0,0);
		else
			curve.rotate = exp(theta*axis.normalize());
		curve.translate = pos;

		splineInfo.push_back(curve);
	}		
	
}


