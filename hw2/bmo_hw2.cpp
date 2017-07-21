#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mathclass.h"

const long double pi = 3.14159265;
double shoulder = 60, elbow = 45, fing1 = -30, fing2 = 0, fing3 = 30;
double rsh = 20, lsh = 20;
double lhip = 20, lknee = 40, lankle = 90;
double rhip = -20, rknee = 40, rankle = 90;

//GLdouble convX1, convY1, convZ1, convX2, convY2, convZ2;
jhm::position P(0,0,10), O(0,0,0);
jhm::vector N, R;
jhm::vector U(0,1,0);
int WIDTH, HEIGHT;
double old_mouseX, old_mouseY, new_mouseX, new_mouseY;
const double FOV_angle = 45.0;
double trackball_radius;


void Circle(double radius)
{	
	const int slice = 32;
	glBegin(GL_POLYGON);
	for (int i=0; i<slice; i++)
	{
		glVertex3f(radius * cos(i*2*pi/slice), radius * sin(i*2*pi/slice), 0.0);
	}
	glEnd();
}

void WireRoundEdgeCube(double x_size, double y_size, double z_size, double radius)
{
	x_size *= 1.001;
	y_size *= 1.001;
	z_size *= 1.001;
	radius *= 1.001;
	const int slice = 32;
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();
		for (int z=-1; z<=1; z+=2)
		{
			glPushMatrix();
				glTranslatef(0.0, 0.0, z*z_size/2);
				glPushMatrix();
					glBegin(GL_POLYGON);
					int sx = 1, sy = 1;
					for (int i=0; i<slice; i++)
					{
						glVertex3f(radius * cos(i*2*pi/slice) + sx*(x_size - 2*radius)/2,
								radius * sin(i*2*pi/slice) + sy*(y_size - 2*radius)/2,
								0.0);
						if ((i+1) % (slice/4) == 0)
						{
							sx *= -1;
						}
						if ((i+1) % (slice/2) == 0)
						{
							sy *= -1;
							sx *= -1;
						}
					}
					glEnd();
				glPopMatrix();
			glPopMatrix();
		}
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void SolidRoundEdgeCube(double x_size, double y_size, double z_size, double radius)
{
	glPushMatrix();
		glPushMatrix();
			glScalef(x_size - 2*radius, y_size, z_size);
			glutSolidCube(1.0);
		glPopMatrix();

		glPushMatrix();
			glScalef(x_size, y_size - 2*radius, z_size);
			glutSolidCube(1.0);
		glPopMatrix();

		for (int i=-1; i<=1; i+=2)
		{
			for (int j=-1; j<=1; j+=2)
			{
				glPushMatrix();
					glTranslatef(i * (x_size - 2*radius)/2, j * (y_size - 2*radius)/2, 0.0);
					glPushMatrix();
						glTranslatef(0.0, 0.0, -z_size/2);
						Circle(radius);
					glPopMatrix();
					glPushMatrix();
						glTranslatef(0.0, 0.0, z_size/2);
						Circle(radius);
					glPopMatrix();
					glTranslatef(0.0, 0.0, -z_size/2);
					GLUquadricObj *quadr = gluNewQuadric();	
					gluCylinder(quadr, radius, radius, z_size, 32, 32);
				glPopMatrix();
			}
		}
	glPopMatrix();
}


void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

jhm::position mouse2world(double x, double y)
{
	jhm::vector N = (O-P).normalize();
	R = (N*U).normalize();
	U = (R*N).normalize();
	double ratio = 2*tan(FOV_angle/2.0 * pi/180.0);
	return P + 1.0*N 
		+ ratio / HEIGHT * x * R 
		+ ratio / HEIGHT * y * U;

}

void display(void)
{
	const int slice = 32;
	static double angle = 0.0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();	
	
	gluLookAt(P.x(), P.y(), P.z(),
			O.x(), O.y(), O.z(),
			0.0, 1.0, 0.0);

	

	glPushMatrix();
		jhm::position now = mouse2world(new_mouseX, new_mouseY);

		jhm::position b = now;

		glColor3f(0, 0x100, 0);

		glLineWidth(10.0);
		glBegin(GL_LINES);
			glVertex3f(b.x(), b.y(), b.z());
			glVertex3f((b+(b-P)).x(), (b+(b-P)).y(), (b+(b-P)).z());//(P.x(), P.y(), P.z());
		glEnd();
		
		/*glPushMatrix();
			glTranslatef(b.x(), b.y(), b.z());
			glutSolidSphere(0.01, 32, 32);
		glPopMatrix();
		glPushMatrix();
			glTranslatef((b+(b-P)).x(), (b+(b-P)).y(), (b+(b-P)).z());
			glutSolidSphere(0.01, 32, 32);
		glPopMatrix();*/
	glPopMatrix();

	glPushMatrix();
		now = mouse2world(new_mouseX, new_mouseY);

		b = now;

		glColor3f(0x100, 0, 0);
		glLineWidth(1.0);
		glutWireSphere(trackball_radius, 32, 32);

		glColor3f(0, 0, 0x100);
		
		glBegin(GL_LINES);
			glVertex3f(O.x(), O.y(), O.z());
			glVertex3f(b.x(), b.y(), b.z());
		glEnd();

	glPopMatrix();




	glRotatef(angle, 1.0, 0.0, 0.0);

	glPushMatrix();
		glColor3f(0x07/(double)0x100, 0x56/(double)0x100, 0x36/(double)0x100);
		glLineWidth(2.0);
		WireRoundEdgeCube(1.4, 1.85, 1.0, 0.09);
		glColor3f(0x6f/(double)0x100, 0xca/(double)0x100, 0xa7/(double)0x100);
		SolidRoundEdgeCube(1.4, 1.85, 1.0, 0.09);
		glPushMatrix();
			glTranslatef(0.0, 1.85/2 - 0.85/2 - 0.125, 0.251);
			glColor3f(0x07/(double)0x100, 0x56/(double)0x100, 0x36/(double)0x100);
			WireRoundEdgeCube(1.4*0.78, 0.85, 0.5, 0.06);
			glColor3f(0xd7/(double)0x100, 0xfa/(double)0x100, 0xe2/(double)0x100);
			SolidRoundEdgeCube(1.4*0.78, 0.85, 0.5, 0.06);
			
			glColor3f(0x00, 0x0e/(double)0x100, 0x00);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLineWidth(1.0);
			glPushMatrix();
				glTranslatef((0.5-0.28)*1.4*0.78, 
						(0.5-0.35)*0.85, 
						0.251);
				Circle(0.03);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(-(0.5-0.28)*1.4*0.78, 
						(0.5-0.35)*0.85, 
						0.251);
				Circle(0.03);
			glPopMatrix();
			glPushMatrix();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glTranslatef(0.0, (0.5-0.42)*0.85-0.15/2, 0.251);
				glBegin(GL_POLYGON);
				for (int i=slice/2; i<=slice; i++)
				{
					glVertex3f(0.15/2 * cos(i*2*pi/slice), 0.15/2 * sin(i*2*pi/slice), 0.0);
				}
				glEnd();
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-(1.4/2 - 0.75*1.4*0.28 - 1.4/8), 
					1.85/2 - 0.85 - 2*0.125 - 0.035/2, 
					0.501);
			glColor3f(0x1a/(double)0x100, 0x53/(double)0x100, 0x4d/(double)0x100);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
				glVertex3f(-0.75*1.4*0.28, 0.035, 0.0);
				glVertex3f(0.75*1.4*0.28, 0.035, 0.0);
				glVertex3f(0.75*1.4*0.28, -0.035, 0.0);
				glVertex3f(-0.75*1.4*0.28, -0.035, 0.0);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.4/2 - 0.1  - 1.4/8, 
					1.85/2 - 0.85 - 2*0.125 - 0.02, 
					0.501);
			glColor3f(0x14/(double)0x100, 0x00/(double)0x100, 0xb0/(double)0x100);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			Circle(0.05);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-(1.4/2 - 0.2 - 1.4/8), 
					1.85/2 - 0.85 - 2*0.125 - 0.17 - 0.065*1.5/2, 
					0.501);
			glColor3f(0xe8/(double)0x100, 0xd2/(double)0x100, 0x2e/(double)0x100);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glScalef(0.065,0.065,0.0);
			glBegin(GL_POLYGON);
				glVertex3f(-0.5, 0.5, 0.0);
				glVertex3f(-0.5, 1.5, 0.0);
				glVertex3f(0.5, 1.5, 0.0);
				glVertex3f(0.5, 0.5, 0.0);
				glVertex3f(1.5, 0.5, 0.0);
				glVertex3f(1.5, -0.5, 0.0);
				glVertex3f(0.5, -0.5, 0.0);
				glVertex3f(0.5, -1.5, 0.0);
				glVertex3f(-0.5, -1.5, 0.0);
				glVertex3f(-0.5, -0.5, 0.0);
				glVertex3f(-1.5, -0.5, 0.0);
				glVertex3f(-1.5, 0.5, 0.0);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-(1.4/2 - 1.4/8 - 0.16/2),
					1.85/2 - 0.85 - 2*0.125 - 0.17 - 0.065*1.5 - 0.21,
					0.251);
			glColor3f(0x14/(double)0x100, 0x00/(double)0x100, 0xb0/(double)0x100);
			SolidRoundEdgeCube(0.16, 0.05, 0.501, 0.025);
			
			glTranslatef(0.21, 0.0, 0.0);
			glColor3f(0x14/(double)0x100, 0x00/(double)0x100, 0xb0/(double)0x100);
			SolidRoundEdgeCube(0.16, 0.05, 0.501, 0.025);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.4/2 - 1.4/8 - 0.25,
					1.85/2 - 0.85 - 2*0.125 - 0.17 - 0.03,
					0.501);
			glColor3f(0x15/(double)0x100, 0xda/(double)0x100, 0xd1/(double)0x100);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(-0.07, -0.119, 0.0);
				glVertex3f(0.07, -0.119, 0.0);	
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.4/2 - 1.4/8 - 0.15,
					1.85/2 - 0.85 - 2*0.125 - 0.17 - 0.26 - 0.18/2 + 0.03,
					0.501);
			glColor3f(0xe9/(double)0x100, 0x00/(double)0x100, 0x4f/(double)0x100);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			Circle(0.18/2);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.4/2 - 1.4/8 - 0.03,
					1.85/2 - 0.85 - 2*0.125 - 0.17 - 0.17 - 0.1/2 + 0.03,
					0.501);
			glColor3f(0x15/(double)0x100, 0xd3/(double)0x100, 0x26/(double)0x100);
			Circle(0.1/2);
		glPopMatrix();

	glPopMatrix();

	glColor3f(0x03/(double)0x100, 0x1e/(double)0x100, 0x33/(double)0x100);
	glPushMatrix();
		glLineWidth(7.0);	
		glRotatef(90, 0.0, 1.0, 0.0);
		glRotatef(90, 0.0, 0.0, -1.0);
		glTranslatef(-0.2, -0.12, 0.701);
			
		glScalef(0.0025, 0.0025, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'B');
		glScalef(10.0/35, 10.0/35, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glScalef(35/10.0, 35/10.0, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'M');
		glScalef(10.0/35, 10.0/35, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glScalef(35/10.0, 35/10.0, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'O');
	glPopMatrix();
	glPushMatrix();
		glLineWidth(7.0);
		glRotatef(90, 0.0, -1.0, 0.0);
		glRotatef(90, 0.0, 0.0, -1.0);
		glTranslatef(-0.2, -0.12, 0.701);
			
		glScalef(0.0025, 0.0025, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'B');
		glScalef(10.0/35, 10.0/35, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glScalef(35/10.0, 35/10.0, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'M');
		glScalef(10.0/35, 10.0/35, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glScalef(35/10.0, 35/10.0, 1.0);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'O');
	glPopMatrix();

	glColor3f(0x63/(double)0x100, 0xb3/(double)0x100, 0xb2/(double)0x100);
	GLUquadricObj *quadr;
	glPushMatrix();
		glTranslatef(1.4/2+0.07, -0.495, 0.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		glRotatef(rsh, 0.0, 0.0, -1.0);
		glRotatef(shoulder, 1.0, 0.0, 0.0);	
		glutSolidSphere(0.07, 32, 32);
		quadr = gluNewQuadric();	
		gluCylinder(quadr, 0.07, 0.07, 0.4, 32, 32);
		glPushMatrix();
			glTranslatef(0.0, 0.0, 0.4);
			glRotatef(elbow, 0.0, -1.0, 0.0);
			glutSolidSphere(0.07, 32, 32);
			quadr = gluNewQuadric();	
			gluCylinder(quadr, 0.07, 0.07, 0.4, 32, 32);
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.4);
				glRotatef(fing1, 1.0, 0.0, 0.0);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.02, 0.02, 0.2, 32, 32);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.4);
				glRotatef(fing2, 1.0, 0.0, 0.0);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.02, 0.02, 0.2, 32, 32);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.4);
				glRotatef(fing3, 1.0, 0.0, 0.0);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.02, 0.02, 0.2, 32, 32);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-1.4/2-0.07, -0.495, 0.0);
		glRotatef(90, 0.0, -1.0, 0.0);
		glRotatef(lsh, 0.0, 0.0, 1.0);
		glRotatef(shoulder, 1.0, 0.0, 0.0);	
		glutSolidSphere(0.07, 32, 32);
		quadr = gluNewQuadric();	
		gluCylinder(quadr, 0.07, 0.07, 0.4, 32, 32);
		glPushMatrix();
			glTranslatef(0.0, 0.0, 0.4);
			glRotatef(elbow, 0.0, 1.0, 0.0);
			glutSolidSphere(0.07, 32, 32);
			quadr = gluNewQuadric();	
			gluCylinder(quadr, 0.07, 0.07, 0.4, 32, 32);
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.4);
				glRotatef(fing1, 1.0, 0.0, 0.0);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.02, 0.02, 0.2, 32, 32);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.4);
				glRotatef(fing2, 1.0, 0.0, 0.0);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.02, 0.02, 0.2, 32, 32);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.4);
				glRotatef(fing3, 1.0, 0.0, 0.0);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.02, 0.02, 0.2, 32, 32);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();


	glPushMatrix();
		glTranslatef(0.3, -1.85/2, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glRotatef(rhip, -1.0, 0.0, 0.0);
		glutSolidSphere(0.07, 32, 32);
		quadr = gluNewQuadric();	
		gluCylinder(quadr, 0.07, 0.07, 0.3, 32, 32);
		glPushMatrix();
			glTranslatef(0.0, 0.0, 0.3);
			glRotatef(rknee, 1.0, 0.0, 0.0);
			glutSolidSphere(0.07, 32, 32);
			quadr = gluNewQuadric();	
			gluCylinder(quadr, 0.07, 0.07, 0.3, 32, 32);
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.3);
				glRotatef(rankle, -1.0, 0.0, 0.0);
				glutSolidSphere(0.07, 32, 32);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.07, 0.03, 0.2, 32, 32);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();	
	glPushMatrix();
		glTranslatef(-0.3, -1.85/2, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glRotatef(lhip, -1.0, 0.0, 0.0);
		glutSolidSphere(0.07, 32, 32);
		quadr = gluNewQuadric();	
		gluCylinder(quadr, 0.07, 0.07, 0.3, 32, 32);
		glPushMatrix();
			glTranslatef(0.0, 0.0, 0.3);
			glRotatef(lknee, 1.0, 0.0, 0.0);
			glutSolidSphere(0.07, 32, 32);
			quadr = gluNewQuadric();	
			gluCylinder(quadr, 0.07, 0.07, 0.3, 32, 32);
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.3);
				glRotatef(lankle, -1.0, 0.0, 0.0);
				glutSolidSphere(0.07, 32, 32);
				quadr = gluNewQuadric();	
				gluCylinder(quadr, 0.07, 0.03, 0.2, 32, 32);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();






	glPopMatrix();
	glutSwapBuffers();

	//angle += 0.1;
}

void changeSize(int w, int h)
{
	if(h==0)
		h=1;

	double ratio = (double)w / h;

	WIDTH = w;
	HEIGHT = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	gluPerspective(FOV_angle, ratio, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void procNormalKeys(unsigned char key, int x, int y)
{
	int delta = 10;
	
	if (key == 'o')
	{
		if (lsh < 90)
		{
			lsh += delta/2;
		}
	}
	else if (key == 'O')
	{
		if (lsh > -90)
		{
			lsh -= delta/2;

		}
	}
	else if (key == 'p')
	{
		if (rsh < 90)
		{
			rsh += delta/2;
		}
	}
	else if (key == 'P')
	{
		if (rsh > -90)
		{
			rsh -= delta/2;

		}
	}

	else if (key == 'h')
	{
		if (shoulder < 90)
		{
			shoulder += delta/2;
		}
	}
	else if (key == 'H')
	{
		if (shoulder > -90)
		{
			shoulder -= delta/2;
		}
	}
	else if (key == 'j')
	{
		if (elbow < 90)
		{
			elbow += delta/2;
		}
	}
	else if (key == 'J')
	{
		if (elbow > 0)
		{
			elbow -= delta/2;
		}
	}
	else if (key == 'k')
	{
		if (fing3 < 90)
		{
			fing1 -= delta/2;
			fing3 += delta/2;
		}
	}
	else if (key == 'K')
	{
		if (fing3 > 0)
		{
			fing1 += delta/2;
			fing3 -= delta/2;
		}
	}
	else if (key == 'i')
	{
		static int lhsign = 1, rhsign = -1;
		static int lksign = -1, rksign = -1;
		
		if (lhip >= 30 || lhip <= -30)
		{
			lhsign *= -1;
		}
		lhip += lhsign*delta/2;

		if (rhip >= 30 || rhip <= -30)
		{
			rhsign *= -1;
		}
		rhip += rhsign*delta/2;

		if (lhsign < 0)
		{
			lknee = 0;
		}
		else
		{
			if (lknee <= 0 || lknee >= 72)
				lksign *= -1;
			lknee += lksign*12;
		}
		
		if (rhsign < 0)
		{
			rknee = 0;
		}
		else
		{
			if (rknee <= 0 || rknee >= 72)
				rksign *= -1;
			rknee += rksign*12;
		}

	}
}

void procSpecialKeys(int key, int x, int y)
{
	if (key == 27)
		exit(0);
}

void procMouse(int button, int state, int x, int y)
{
	/*double modelMatrix[4*4];
	double projMatrix[4*4];
	GLint view[4];

	//float fx = 2*x / (double)WIDTH - 1;
	//float fy = 1 - (2*y) / (double)HEIGHT;
	*/

	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			old_mouseX = (- (WIDTH/2.0 - x));
			old_mouseY = (HEIGHT/2.0 - y);
			/*glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
			glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
			glGetIntegerv(GL_VIEWPORT, view);
			//y = view[3] - y - 1;
			gluUnProject(fx, fy, 0.0,
					modelMatrix, projMatrix, view,
					&convX1, &convY1, &convZ1);
			gluUnProject(fx, fy, 1.0,
					modelMatrix, projMatrix, view,
					&convX2, &convY2, &convZ2);
			printf("converted: %f %f %f -> %f %f %f\n",convX1, convY1, convZ1,convX2, convY2, convZ2);*/

		}
	}
}

void procPassiveMouseMove(int x, int y)
{
}

double solve_quadratic_min(double a, double b, double c)
{
	return (- b - sqrt(b*b-4*a*c)) / (2*a);
}

double get_min_rad(double a, double b, double c)
{
	return c - b*b/(4*a);
}

bool have_solution(double a, double b, double c)
{
	if(b*b-4*a*c >= 0)
		return true;
	return false;
}

void procActiveMouseMove(int x, int y)
{
	//printf("active move: %d %d\n",x,y);
	new_mouseX = (- (WIDTH/2.0 - x));
	new_mouseY = (HEIGHT/2.0 - y);
	
	jhm::position F = mouse2world(old_mouseX, old_mouseY);
	jhm::position M = mouse2world(new_mouseX, new_mouseY);

	trackball_radius = 0.5 * get_min_rad((F-P)%(F-P), (F-P)%(P-O), (P-O)%(P-O));
	printf("rad: %f\n",trackball_radius);
	if (have_solution((F-P)%(F-P), (F-P)%(P-O), (P-O)%(P-O) - trackball_radius*trackball_radius))
	{
		double at = solve_quadratic_min((F-P)%(F-P), (F-P)%(P-O), (P-O)%(P-O) - trackball_radius*trackball_radius);
		double bt = solve_quadratic_min((M-P)%(M-P), (M-P)%(P-O), (P-O)%(P-O) - trackball_radius*trackball_radius);
		printf("at: %f  bt:%f\n",at,bt);
		jhm::vector a = P + at*(F-P) - O;
		jhm::vector b = P + bt*(M-P) - O;

		double len = a.length()*b.length();
		jhm::vector cross = a*b;
		double sin_th = cross.length() / len;
		double cos_th = a%b / len;
		double rot_th = atan2(sin_th, cos_th);
		printf("theta: %f\n",rot_th);
		jhm::vector rot_axis = cross.normalize();

		P = rotate(exp(rot_th * rot_axis), P);
		U = rotate(exp(-rot_th * rot_axis), U);
	}
	old_mouseX = new_mouseX;
	old_mouseY = new_mouseY;
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize(400, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("BMO");
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutIdleFunc(display);

	glutMouseFunc(procMouse);
	glutMotionFunc(procActiveMouseMove);
	glutPassiveMotionFunc(procPassiveMouseMove);
	glutKeyboardFunc(procNormalKeys);
	glutSpecialFunc(procSpecialKeys);

	glEnable(GL_DEPTH_TEST);
	glutMainLoop();

	return 0;
}
