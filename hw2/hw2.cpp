#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include "mathclass.h"
using namespace jhm;

vector getCameraPos();

static float radius = 5;

position camOrigin(0, 0, 0);				// focus point of camera
position camPosition(0, 0, -10);		// default camera position vector
										// (from cuurent camera position to focus.)

vector camUp(0, 1, 0);				// current camera up vector
vector camRight(1, 0, 0);

double dist = 10.0;						// camera distance
double zoom = 90.0;						// zoom angle(FOV)

position currOri(0, 0, 0);
position nextOri(0, 0, 0);
vector rotVec(0.0, 0.0, 1.0);
double rotAngle = 0.0;

// temp vectors to watch matching points
position matchT1(0, 0, 0);
position matchT2(0, 0, 0);

bool isZoom = false;
bool isRotate = true;
bool isSeekObject = false;
bool isSeekPoint = false;
bool initFlag = true;
int obj_list_idx;



vector getBaseVector(vector proj);
vector getRotVector(vector v0, vector v1);
double getRotAngle(vector v0, vector v1);



class Drawee
{
	private:
		double mat[4][4];
	public:
		Drawee(GLfloat mat[16], double size) {
			size *= 1.5;
			for(int i = 0 ; i<4; i++){
				for(int j = 0; j<4; j++){
					this->mat[i][j] = mat[j*4+i];
				}
			}
			this -> pos = mat_mul(0, 0, 0, 1) + position(0,0,0);
			this -> size = mat_mul(1, 0, 0, 0).length()*size;
		}
		void addCurrPos(GLfloat mat[16], double size) {
			for(int i = 0 ; i<4; i++){
				for(int j = 0; j<4; j++){
					this->mat[i][j] = mat[j*4+i];
				}
			}
			this -> currPos = mat_mul(0, 0, 0, 1) + position(0,0,0);
		}
		vector mat_mul(double a, double b, double c, double d) {
			double in[4] = {a,b,c,d};
			double out[3];
			for(int i = 0; i<3; i++) {
				double sum = 0;
				for(int j = 0; j<4; j++)
					sum += mat[i][j]*in[j];
				out[i] = sum;
			}
			return vector(out);
		}
		position pos;
		position currPos;
		double size;
};

std::vector<Drawee> obj_list;

void addDrawee(double size) {
	GLfloat matrix[16];

	if (initFlag) {
		glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
		obj_list.push_back(Drawee(matrix, size));
	}
	else {
		glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
		obj_list[obj_list_idx++].addCurrPos(matrix, size);

	}
}


void init(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glShadeModel(GL_FLAT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();



	gluLookAt(camPosition.x(), camPosition.y(), camPosition.z(), 
			camOrigin.x(), camOrigin.y(), camOrigin.z(), 
			camUp.x(), camUp.y(), camUp.z());
	
	

	/*vector v0 = getBaseVector(currOri - camPosition);
	vector v1 = getBaseVector(nextOri - camPosition);
	
	if (v0.length()!=0 && v1.length()!=0)
	{
		position focus = camOrigin;
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3d(focus.x(), focus.y(), focus.z());
		glVertex3d(focus.x()+v0.x(), focus.y()+v0.y(), focus.z()+v0.z());
		glEnd();
		
		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 0.0);
		glVertex3d(focus.x(), focus.y(), focus.z());
		glVertex3d(focus.x()+v1.x(), focus.y()+v1.y(), focus.z()+v1.z());
		glEnd();

		vector v2 = v1*v0;//getRotVector(v1, v0);
		v2*=2000;
		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 1.0);
		glVertex3d(focus.x(), focus.y(), focus.z());
		glVertex3d(focus.x()+v2.x(), focus.y()+v2.y(), focus.z()+v2.z());
		glEnd();

	}*/
	glLineWidth(1.0f);

	glInitNames();


	obj_list_idx = 0;

	glPushMatrix();
	// draw a cube
	glPushName(0);
	glPushMatrix();
	glScalef(4.0, 4.0, 4.0);
	glColor3f(0.1, 0.08, 0.09);
	glutWireCube(1.005);
	addDrawee(1.005);
	glPopName();

	glPushName(1);
	glColor3f(0.38, 0.93, 0.5);
	glutSolidCube(1.0);
	addDrawee(1.0);
	glPopName();

	glPushName(2);
	glPushMatrix();
	glTranslatef(0.7, 0.7, 0.7);
	glColor3f(0.1, 0.08, 0.09);
	glutWireCube(1.005);
	addDrawee(1.005);
	glPopName();

	glPushName(3);
	glColor3f(0.38, 0.93, 0.5);
	glutSolidCube(1.0);
	addDrawee(1.0);
	glPopMatrix();
	glPopName();

	glPopMatrix();
	glPopMatrix();

	// draw trackball(?)
	glPushName(4);
	glPushMatrix();
	glTranslatef(camOrigin.x(), camOrigin.y(), camOrigin.z());
	glScalef(4.0, 4.0, 4.0);
	glColor3f(1.0, 1.0, 1.0);
	glutWireSphere(1.25, 50, 10);
	addDrawee(1.25);
	glPopName();

	glPopMatrix();

	glPopMatrix();
	glPushMatrix();

	// (should be)mouse projection point
	/*glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(0.8, 0.8, 0.2);
	glVertex3d(matchT1.x(), matchT1.y(), matchT1.z());
	glColor3f(0.2, 0.8, 0.2);
	glVertex3d(matchT2.x(), matchT2.y(), matchT2.z());
	glEnd();*/

	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	
	initFlag = false;
}

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	
	gluPerspective(zoom, 1, radius, 50);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}



double getSqrt(double f){
	if(f<0) return -1;
	else return sqrt(f);
}

vector getBaseVector(vector proj){
	// t*mouseOri poses on the sphere
	position C = camPosition; //camera is now at this postion
	position F = camOrigin;
	vector P = proj; //vector from camera postion toward the mouse world coordinate

	double a = P%P, b = P%(C-F), c = (C-F)%(C-F) - radius*radius;
	double rootVal = getSqrt(b*b - a*c);

	// mouse out of trackball
	if(rootVal < 0) return vector(0,0,0);

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
double getRotAngle(vector v0, vector v1){
//	v0 = getBaseVector_(v0);
//	v1 = getBaseVector_(v1);

	vector cross = v0*v1;
	double dot = v0%v1;

	double norm0 = v0.length();
	double norm1 = v1.length();

	double cos = dot/(norm0*norm1);
	double sin = cross.length()/(norm0*norm1);

//	cout << "atan2 : " << atan2(sin, cos) << endl;
	float angle = atan2(sin, cos);
	return angle;
}
//

vector getRotVector(vector v0, vector v1){
	vector cross = v0*v1;
	vector res = cross.normalize() * 0.5*getRotAngle(v0, v1);

	return res;
}

vector getMousePos(int x, int y){
	double ratio = 400/((dist-radius)*tan(zoom/2 /180.0 * 3.14159265));
	return vector(((float)x - 400)/ratio, -((float)y - 400)/ratio, 0);
}

vector getPlane(){
	return (camPosition - camOrigin).normalize() * radius;
}

//code from http://www.gisdeveloper.co.kr/?p=53
int SelectObjects(GLint x, GLint y)
{
	int ret = -1;
	GLuint selectBuff[64];	
	GLint hits, viewport[4];

	glSelectBuffer(64, selectBuff);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix(x, viewport[3]-y, 2, 2, viewport);
	gluPerspective(zoom, 1, radius, 50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display();
	hits = glRenderMode(GL_RENDER);
	if(hits>0) {
		ret = selectBuff[3];
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	return ret;
}

void mouseMove(int x, int y){	
	vector mousePos = getMousePos(x, y);	
	nextOri = camOrigin + getPlane() - mousePos.x()*camRight + mousePos.y()*camUp;

	//cout << "nextOri : " << nextOri << endl;
	if (isRotate) {
		vector v0 = getBaseVector(currOri - camPosition);
		vector v1 = getBaseVector(nextOri - camPosition);
		
		if (v0.length()!=0 && v1.length()!=0) {
			vector V = camPosition - camOrigin;
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

	//currOri = nextOri;
}

void mouseButton(int button, int state, int x, int y) {
	double zoomDelta = 0.1;
	double dollyDelta = 0.1;

	if (button == GLUT_LEFT_BUTTON) {
		// when the button is released
		if (state == GLUT_UP) {
		}
		else  {// state = GLUT_DOWN
			if(isSeekObject) {
				int obj_idx = SelectObjects(x,y);
				//std::cout << obj_idx << std::endl;
				if (obj_idx < 0)
					return;
				position obj_pos = obj_list[obj_idx].currPos;
				//std::cout << obj_pos << std::endl;
				vector V = camPosition - obj_pos.z()*((camOrigin-camPosition).normalize())
										- obj_pos.x()*camRight
										+ obj_pos.y()*camUp
										- camOrigin;
				camPosition += V;
				camOrigin += V;
			}
			else if(isSeekPoint) {
				GLfloat zCurs;
				GLint view[4];
				GLdouble model[16];
				GLdouble proj[16];
				glGetDoublev(GL_MODELVIEW_MATRIX, model);
				glGetDoublev(GL_PROJECTION_MATRIX, proj);
				glGetIntegerv(GL_VIEWPORT, view);
				glReadPixels(x, view[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCurs);
				double px, py, pz;
				gluUnProject(x, view[3]-y, zCurs, model, proj, view, &px, &py, &pz);
				position pos(px, py, pz);
				//std::cout << pos << std::endl;
				if(pz <= -50)
					return;
				
				//vector mouse = zCurs/(dist-radius) * getMousePos(x,y);
				//pos = position(mouse.x(), mouse.y(), -zCurs);

				//std::cout << pos << std::endl;
				vector V = camPosition - pos.z()*((camOrigin-camPosition).normalize())
										- pos.x()*camRight
										+ pos.y()*camUp
										- camOrigin;
				camPosition += V;
				camOrigin += V;

			}
			else {
				vector mousePos = getMousePos(x, y);
				currOri = camOrigin + getPlane() - mousePos.x()*camRight + mousePos.y()*camUp;
			}
		}
	}
	else if (button == 3) {
		if (isZoom) {
			if (zoom > 0 +  zoomDelta)
				zoom -= zoomDelta;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(zoom, 1, radius, 50);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

		}
		else {
			vector V = camPosition - camOrigin;
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
			gluPerspective(zoom, 1, radius, 50);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();


		}
		else {
			vector V = camPosition - camOrigin;
			V = V.normalize() * (V.length() + dollyDelta);
			camPosition = camOrigin + V;
			dist += dollyDelta;

		}
	}

	glutPostRedisplay();

}

double max2(double a, double b) {
	return a>b?a:b;
}

double max3(double a, double b, double c) {
	return max2(max2(a,b),c);
}

double abs(double a) {
	return a>0?a:-a;
}

void keyboardDown(unsigned char key, int x, int y) {
	if (key == 'z') //zoom
		isZoom = true;
	else if (key == 't') //translate
		isRotate = false;
	else if (key == 'a') {//show all
		vector V = camOrigin - position(0,0,0);
		camPosition -= V;
		camOrigin -= V;	
		
		double t_max = 0;
		for(int i = 0; i<(int)obj_list.size(); i++) {
			

			const double PI = 3.14159265;
			double tx = obj_list[i].pos.x();
			double ty = obj_list[i].pos.y();
			double tz = obj_list[i].currPos.z();
			double tr = obj_list[i].size;
			
			double dx = (tr/cos(zoom/2 * PI/180.0) + tx) / tan(zoom/2 * PI/180.0);
			double dy = (tr/cos(zoom/2 * PI/180.0) + ty) / tan(zoom/2 * PI/180.0);
			double dz = (radius) + obj_list[i].size;

			//std::cout << i << ": " << obj_list[i].pos << " " << obj_list[i].size << std::endl;
			//std::cout << obj_list[i].currPos << std::endl;
			//std::cout << "dx: " << dx << " dy: " << dy << " dz: " << dz << " z: " << abs(tz) << std::endl;
			if (max3(dx,dy,dz) > abs(tz)) {
				if (t_max < max3(dx, dy, dz) - abs(tz)) 
					t_max = max3(dx,dy,dz) - abs(tz);
			}
		}
		//std::cout << t_max << std::endl;
		//std::cout<<std::endl;
		
		V = camPosition - camOrigin;
		V = V.normalize() * (V.length() + t_max);
		camPosition = camOrigin + V;
		dist += t_max;

		glutPostRedisplay();
	}


	else if (key == 'c') {
		for(int i = 0; i<(int)obj_list.size(); i++) {
			std::cout << i << ": " << obj_list[i].pos << " " << obj_list[i].size << std::endl;
		}
		std::cout << std::endl;
	}
	else if (key == 's')
		isSeekObject = true;
	else if (key == 'p')
		isSeekPoint = true;
}

void keyboardUp(unsigned char key, int x, int y) {
	if (key == 'z')
		isZoom = false;
	else if (key == 't')
		isRotate = true;
	else if (key == 's')
		isSeekObject = false;
	else if (key == 'p')
		isSeekPoint = false;
}


int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(400, 400);

	glutCreateWindow("3Dviewer");
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


