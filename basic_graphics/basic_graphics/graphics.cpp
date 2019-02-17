#include "GLUT.H"
#include <math.h>
#include <time.h>
#include <vector>

/*
	Linor Dolev - 201619079
	Roie Danino - 316440296
*/

using namespace std;

const double PI = 3.14;
const double WIDTH = 1200;
const double HEIGHT = 600;

double eyeX = 0, eyeY = 20, eyeZ = 10;
double dx = 0, dy = 0, dz = 0;

double offset = 0;

// motion
double dir[3]; // x,y,z
double sight_angle = PI; // in radians
double pitch = 0;
double speed = 0, angular_speed = 0;


struct 
{
	double x;
	double y;

}typedef Point2D;

vector<Point2D> points;

void init()
{
	glClearColor(0.6, 0.6, 0.6 , 0); // set background color 
}

void drawSplitLineBetween3Dand2D() 
{
	glColor3d(0, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2d(-1, 1);
	glVertex2d(-1, -1);
	glEnd();
}

void drawSplitLineBetweenDrawAndReflection()
{
	glColor3d(1, 1, 1);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2d(0, 1);
	glVertex2d(0, -1);
	glEnd();
}

void drawLines()
{
	const int SIZE = points.size();
	int i;
	
	glLineWidth(3);
	glColor3d(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < SIZE; i++)
	{
		Point2D point = points.at(i);
		glVertex2d(point.x, point.y);
	}
	glEnd();

	glPointSize(7);
	glColor3d(1, 0, 0);
	for (i = 0; i < SIZE; i++)
	{
		Point2D point = points.at(i);
		glBegin(GL_POINTS);
		glVertex2d(point.x, point.y);
		glEnd();
	}
}

void drawCylinder(int n, double topr, double bottomr, double topY, double bottomY)
{
	double alpha, teta = (2 * PI) / n;
	double delta = 0;
	for (alpha = 0; alpha <= 2 * PI + teta; alpha += teta)
	{
		glBegin(GL_POLYGON);//GL_LINE_LOOP);
		glColor3d(1 - 0.7*fabs(sin(alpha)), 1 - fabs(sin(alpha)), (1 + cos(alpha)) / 2);
		glVertex3d(topr*sin(alpha), topY, topr*cos(alpha)); // 1
		glColor3d(1 - 0.7*fabs(sin(alpha + teta)), 1 - fabs(sin(alpha + teta)), (1 + cos(alpha + teta)) / 2);
		glVertex3d(topr*sin(alpha + teta), topY, topr*cos(alpha + teta)); // 2															
		glVertex3d(bottomr*sin(alpha + teta + delta), bottomY, bottomr*cos(alpha + teta + delta)); // 3
		glColor3d(1 - 0.7*fabs(sin(alpha)), 1 - fabs(sin(alpha)), (1 + cos(alpha)) / 2);
		glVertex3d(bottomr*sin(alpha + delta), bottomY, bottomr*cos(alpha + delta)); // 4
		glEnd();
	}

}

void drawRightBackground()
{
	glColor3d(0.4, 0.4, 1);
	glBegin(GL_POLYGON);
		glVertex2d(-1, -1);
		glVertex2d(-1 , 1);
		glVertex2d(1, 1);
		glVertex2d(1, -1);
	glEnd();
}

void rightDisplay() 
{
	drawRightBackground();
	drawSplitLineBetweenDrawAndReflection();
	
	drawLines();

	glPushMatrix();
	glScaled(-1, 1, 1);
	drawLines();
	glPopMatrix();
}

void drawCylinders() 
{
	const int SIZE = points.size();
	int i;

	for (i = 0; i < SIZE - 1; i++) {
		Point2D top = points.at(i);
		Point2D bottom = points.at(i + 1);

		glPushMatrix();
			drawCylinder(100, top.x, bottom.x, top.y, bottom.y);
		glPopMatrix();
	}
}

void leftDisplay()
{
	glMatrixMode(GL_PROJECTION); // start defining the projection matrix
	glLoadIdentity();

	// left, right, bottom, top , near, far
	glFrustum(-1, 1, -1, 1, 0.8, 250);

	gluLookAt(eyeX, eyeY, eyeZ, // eye coordinates
		eyeX + dir[0], eyeY + dir[1], eyeZ + dir[2],  // Point of interest
		0, 1, 0);// UP
				 
	glMatrixMode(GL_MODELVIEW);  // go back to "MODEL-VIEW" matrix
	glLoadIdentity();

	glPushMatrix();
	glTranslated(0, 20, 0);
	glScaled(3, 3, 3);
	drawCylinders();
	glPopMatrix();

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-Buffer

	//2D Paint
	glViewport(WIDTH/2, 0, WIDTH / 2, HEIGHT);
	// prepare 2D 
	glMatrixMode(GL_PROJECTION); // start defining the projection matrix
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glDisable(GL_DEPTH_TEST);
	// now go to model transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawSplitLineBetween3Dand2D();
	
	rightDisplay();

	// 3D View
	glViewport(0, 0, WIDTH / 2, HEIGHT);
	
	leftDisplay();

	glutSwapBuffers();
}

void idle()
{
	offset += 0.3;

	// compute ego-motion
	sight_angle += angular_speed;

	dir[0] = sin(sight_angle);
	dir[1] = sin(pitch);
	dir[2] = cos(sight_angle);

	eyeX += speed*dir[0];
	eyeY += speed*dir[1];
	eyeZ += speed*dir[2];

	glutPostRedisplay(); // call to refresh window
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: // forward
		speed += 0.01;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.01;
		break;
	case GLUT_KEY_LEFT:
		angular_speed += 0.003;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.003;
		break;
	case GLUT_KEY_PAGE_UP:
		pitch += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		pitch -= 0.1;
		break;
	}
}

//transfrom x from [a, b] to [c, d]
double transformRange(double x, double a, double b, double c, double d) 
{
	return (x - a)*((d - c) / (b - a)) + c;
}

void mouse(int button, int state, int x, int y)
{
	double myX = transformRange(x, 0, WIDTH, -3, 1);
	double myY = -((double)y - (HEIGHT / 2.0))/(HEIGHT / 2.0);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && myX > 0)
	{
		Point2D point = { myX, myY };
		points.push_back(point);
		
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
	{
		points.clear();
	}
	
}

/*
	Linor Dolev - 201619079
	Roie Danino - 316440296
*/

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	// add 2 frame buffers and Z-Buffer
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Basic graphics app");

	glutDisplayFunc(display);
	glutSpecialFunc(special); // for non-ascii keys
	glutMouseFunc(mouse);

	glutIdleFunc(idle);

	init();

	glutMainLoop();
}











/*
void display_top()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-Buffer
glViewport(0, 0, WIDTH, HEIGHT);

glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(0, 80, 0, // eye coordinates
0, 30, 0,  // Point of interest
0, 0, -1);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();

glutSwapBuffers();
}

void display_cockpit()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-Buffer
glViewport(0, 0, WIDTH, HEIGHT);

glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(aX, aY + 2, aZ, // eye coordinates
aX + adir[0], aY + 2 + adir[1], aZ + adir[2],  // Point of interest
0, 1, 0);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();
// left mirror view
glViewport(0, 100, 100, 100);
glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(aX, aY + 3, aZ, // eye coordinates
aX - adir[0], aY + 3 - adir[1], aZ - adir[2],  // Point of interest
0, 1, 0);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();

// right mirror view
glViewport(WIDTH - 100, 100, 100, 100);
glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(aX, aY + 3, aZ, // eye coordinates
aX - adir[0], aY + 3 - adir[1], aZ - adir[2],  // Point of interest
0, 1, 0);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();
// Draw compass control
glViewport(WIDTH - 100, 0, 100, 100);
// prepare 2D
glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();
glOrtho(-1, 1, -1, 1, -1, 1);
glDisable(GL_DEPTH_TEST);
// now go to model transformations
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
DrawCompassControl();


glEnable(GL_DEPTH_TEST);


glutSwapBuffers();
}

void display_combined()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-Buffer
// Regular view
glViewport(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);

glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(eyeX, eyeY, eyeZ, // eye coordinates
eyeX + dir[0], eyeY + dir[1], eyeZ + dir[2],  // Point of interest
0, 1, 0);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();

// Top view
glViewport(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(0, 80, 0, // eye coordinates
0, 30, 0,  // Point of interest
0, 0, -1);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();
// Cockpit view
glViewport(0, 0, WIDTH, HEIGHT / 2);

glMatrixMode(GL_PROJECTION); // start defining the projection matrix
glLoadIdentity();

// left, right, bottom, top , near, far
glFrustum(-1, 1, -1, 1, 0.8, 250);

gluLookAt(aX, aY + 2, aZ, // eye coordinates
aX + adir[0], aY + 2 + adir[1], aZ + adir[2],  // Point of interest
0, 1, 0);// UP
// go back to "MODEL-VIEW" matrix
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

DrawView();

glutSwapBuffers();
}
*/
