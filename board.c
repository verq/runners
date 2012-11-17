#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "man.h"

#define UNFILLED		0
#define ORANGE			1
#define GREEN			2

#define RUNNERS_EYES		0
#define BEHING_RUNNERS_HEAD	1


int view_mode;

void display();
void reshape();
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

void draw_board();
void draw_track(double value, int filled);

int main(int argc, char **argv) {
	init_runners();
	
	view_mode = RUNNERS_EYES;

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
	glPushMatrix();
	glLoadIdentity();

	int number_of_runner = 3;

	double angle;
	if (runners[number_of_runner] -> turn_angle == 0) angle = 0;
	else if (runners[number_of_runner] -> turn_angle == 180.0) angle = -PI;
	else angle = -runners[number_of_runner] -> turn_angle * PI / 180.0;
	
	if (view_mode == RUNNERS_EYES) {
		gluLookAt(runners[number_of_runner] -> head_x, runners[number_of_runner] -> head_y, runners[number_of_runner] -> head_z,
			runners[number_of_runner] -> head_x + cos(angle),
			runners[number_of_runner] -> head_y,
			runners[number_of_runner] -> head_z + sin(angle),
			0.0,  1.0,  0.0);
	} else {
		gluLookAt(runners[number_of_runner] -> head_x, runners[number_of_runner] -> head_y, runners[number_of_runner] -> head_z,
			runners[number_of_runner] -> head_x + cos(angle),
			runners[number_of_runner] -> head_y,
			runners[number_of_runner] -> head_z + sin(angle),
			0.0,  1.0,  0.0);
	}
	
	//gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
	//printf("%lf %lf %lf \t %lf %lf %lf \t %lf %lf %lf\n", eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

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

	glPopMatrix();

	glFlush();
	glutSwapBuffers();	
}

void reshape(int w, int h) {
	glPopMatrix();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(100, 1.0, 1, 200);

	glPushMatrix();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27: exit(0); break;
		case 'm': printf("m\n");
			if (view_mode == RUNNERS_EYES) view_mode = BEHING_RUNNERS_HEAD;
			else view_mode = RUNNERS_EYES; break;
	}
}

void keyboard_special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT: break;
		case GLUT_KEY_RIGHT:  break;
		case GLUT_KEY_UP: break;
		case GLUT_KEY_DOWN: break;
	}
}