#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include "hw3.h"
#include "spline.h"

using namespace std;
//using namespace jhm;

SplineType SPLINETYPE;
std::vector<curveInfo> splineInfo;


double D = 1/(double)degree;

void getTextInput();

void init(){
	glClearColor(0.1, 0.1, 0.1, 1.0);
	/*glShadeModel(GL_FLAT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);*/



	//
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//




	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);//
}

void displaySpline(){
	Spline spline(SPLINETYPE, splineInfo);

	spline.drawSpline();
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	gluLookAt(0.0, 0.0, -100.0, 
			0.0, 0.0, 0.0, 
			0.0, 1.0, 0.0);

	glColor3d(0.38, 0.93 ,0.5);

	/*glBegin(GL_QUAD_STRIP);
	 glVertex3f(0.2, 0.2,0.0);
  glVertex3f(0.8, 0.2,0.0);
  glVertex3f(0.2,0.4,0.0);
  glVertex3f(0.8,0.4,0.0);
  glVertex3f(0.2,0.8,0.0);
  glVertex3f(0.8,0.8,0.0);
	glEnd();*/

	// Creating Light Reference : http://www.falloutsoftware.com/tutorials/gl/gl8.htm
	// Create light components
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat specularLight[] = { 0.1f, 0.1f, 0.1f, 0.1f };
	GLfloat position0[] = { -1.0f, -1.0f, -1.0f, 0.0f };
	GLfloat position1[] = { 1, 1, -1, 0};
	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	

	GLfloat mat_specular[] = { -1.0, -1.0, -1.0, 1.0 };
	GLfloat mat_shininess[] = { 10.0 };
	glShadeModel (GL_SMOOTH);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	/*glPushMatrix();
		glColor3f(0,1,0);
		glutSolidSphere(20.0, 50, 100);
	glPopMatrix();*/

	displaySpline();
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(90.0, 1, 5, 500);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
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

/*
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
*/
	glutMainLoop();

	return 0;
}

void getTextInput(){
	string line;
	string splineType;
	
	ifstream data;
	data.open("input.txt");//"data.txt");

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


