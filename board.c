#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "man.h"

#define UNFILLED		0
#define ORANGE			1
#define GREEN			2

GLdouble eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz;

void init_eyes();

void display();
void reshape();
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

void draw_board();
void draw_track(double value, int filled);

int main(int argc, char **argv) {
	init_runners();
	init_eyes();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_special);

	running();
	glutMainLoop();

	free_runners();
 	return 0;
}


void init_eyes() {
	eyex = 3;
	eyey = 3;
	eyez = -3;
	centerx = 2;
	centery = 0;
	centerz = 1;
	upx = 0;
	upy = 1;
	upz = 0;

}

void draw_track(double value, int filled) {
	int number_of_segments = 100;
	double angle, x, z;

	if (filled == UNFILLED) {
		glBegin(GL_LINE_LOOP);		
		glColor3f(0.5, 0.5, 0.5);
	} else {
		glBegin(GL_POLYGON);
		if (filled == GREEN) glColor3f(0, 0.5, 0.1);
		else glColor3f(1, 0.3, 0.1);
	}

	glVertex3f(value, 0, value);
 	glVertex3f(-value, 0, value);

 	for(int i = 0; i < number_of_segments/2; i++) {
 		angle = 2.0 * PI * (i + number_of_segments/4) / number_of_segments;
 		x = value * cos(angle);
 		z = value * sin(angle);
 		glVertex3f(x - value, 0, z);
 	}

	glVertex3f(-value, 0, -value);
 	glVertex3f(value, 0, -value);

 	for(int i = number_of_segments/2 + 1; i <= number_of_segments; i++) {
 		angle = 2.0 * PI * (i + number_of_segments/4) / number_of_segments;
 		x = value * cos(angle);
 		z = value * sin(angle);
 		glVertex3f(x + value, 0, z);
 	}
  	glEnd();
}

void draw_board() {
	double value = 75;
	double shift = 5;
	
	draw_track(value, ORANGE);
	draw_track(value - MAX_NUMBER_OF_RUNERS * shift, GREEN);
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		draw_track(value, UNFILLED);
		value = value - shift;
	}
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

	printf("%lf %lf %lf \t %lf %lf %lf \t %lf %lf %lf\n", eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
	draw_board();

	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3f(0.0, 100.0, 0.0);
	glVertex3f(0.0, -100.0, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(100.0, 0.0, 0.0);
	glVertex3f(-100.0, 0.0, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0.0, 0.0, 100.0);
	glVertex3f(0.0, 0.0, -100.0);
	glEnd();

	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> tree_root != NULL) draw_runner(runners[i]);
	}

	glPushMatrix();

	glFlush();
	glutSwapBuffers();	
}

void reshape(int w, int h) {
	glPopMatrix();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w == 0) w = 1;
	if (h == 0) h = 1;

	double d = 180.0;
	glOrtho( -d/2, d/2,-d/2, d/2, -d*2, d*2);
	//if (w < h) glFrustum(-1.0, 1.0, -1.0 * h / w, 1.0 * h / w, 1.0, 2.0);
	//else glFrustum(-1.0 * w / h, 1.0 * w / h, -1.0, 1.0, 1.0, 2.0);
	glPushMatrix();
	
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27: exit(0); break;
		case 'n': init_eyes(); display(); break;
		case '1': eyex = eyex - 1; display(); break;
		case '2': eyey = eyey - 1; display(); break;
		case '3': eyez = eyez - 1; display(); break;
		case '4': centerx = centerx - 1; display(); break;
		case '5': centery = centery - 1; display(); break;
		case '6': centerz = centerz - 1; display(); break;
		case '7': upx = upx - 1; display(); break;
		case '8': upy = upy - 1; display(); break;
		case '9': upz = upz - 1; display(); break;
		case 'q': eyex = eyex + 1; display(); break;
		case 'w': eyey = eyey + 1; display(); break;
		case 'e': eyez = eyez + 1; display(); break;
		case 'r': centerx = centerx + 1; display(); break;
		case 't': centery = centery + 1; display(); break;
		case 'y': centerz = centerz + 1; display(); break;
		case 'u': upx = upx + 1; display(); break;
		case 'i': upy = upy + 1; display(); break;
		case 'o': upz = upz + 1; display(); break;
	}
}

void keyboard_special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT: eyey = eyey - 1; centery = centery - 1; display(); break;
		case GLUT_KEY_RIGHT: eyey = eyey + 1; centery = centery + 1; display(); break;
		case GLUT_KEY_UP: upz = upz + 1; display(); break;
		case GLUT_KEY_DOWN: upz = upz - 1; display(); break;
	}
}