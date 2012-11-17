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
int number_of_runner;
int width, height;


void display();
void display_runners_view();
void display_top_view();

void reshape();
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

void draw_board();
void draw_track(double value, int filled);

int main(int argc, char **argv) {
	init_runners();
	view_mode = BEHING_RUNNERS_HEAD;
	number_of_runner = 3;
	
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

void display_runners_view() {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glViewport(0, 0, width, height);
	gluPerspective(100, 1.0, 1, 200);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Man* looker = runners[number_of_runner];
	
	double angle;
	if (looker -> turn_angle == 0) angle = 0;
	else if (looker -> turn_angle == 180.0) angle = -PI;
	else angle = -looker -> turn_angle * PI / 180.0;

	if (view_mode == RUNNERS_EYES) {
		gluLookAt(looker -> head_x, looker -> head_y, looker -> head_z,
			looker -> head_x + cos(angle),
			looker -> head_y,
			looker -> head_z + sin(angle),
			0.0,  1.0,  0.0);
	} else {
		double behind_shift = 10.0;
		if (looker -> running_phase == FORWARD) {
			gluLookAt(looker -> head_x - behind_shift, looker -> head_y, looker -> head_z,
				looker -> head_x + cos(angle),
				looker -> head_y,
				looker -> head_z + sin(angle),
				0.0,  1.0,  0.0);
		} else if (looker -> running_phase == FORWARD_TURN || looker -> running_phase == BACKWARD_TURN) {
			gluLookAt(looker -> head_x - behind_shift * cos(angle), looker -> head_y, looker -> head_z - behind_shift * sin(angle),
				looker -> head_x - cos(angle),
				looker -> head_y,
				looker -> head_z - sin(angle),
				0.0,  1.0,  0.0);
		} else if (looker -> running_phase == BACKWARD) {
			gluLookAt(looker -> head_x + behind_shift, looker -> head_y, looker -> head_z,
				looker -> head_x + cos(angle),
				looker -> head_y,
				looker -> head_z + sin(angle),
				0.0,  1.0,  0.0);
		}
	}
	draw_board();

	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> tree_root != NULL) draw_runner(runners[i]);
	}
	glPopMatrix();

}

void display_top_view() {
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	glLoadIdentity();

	glViewport(1 * width / 3, 1 * height / 4, width, height);
	gluPerspective(100, 1.0, 1, 300.0);
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(-50, 250, -70, -50, 0, -70, 0, 0, 1);
	draw_board();

	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> tree_root != NULL) draw_runner(runners[i]);
	}
	
	glPopMatrix();
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	display_runners_view();
	display_top_view();
	glFlush();
	glutSwapBuffers();

}

void reshape(int w, int h) {
	width = w;
	height = h;
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, w, h);
	glLoadIdentity();
	gluPerspective(100, 1.0, 1, 300);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
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