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
int width, height;
double start_position = 35.0;

/* DRAWING BOARD */
void draw_board_and_runners();
void draw_board();
void draw_track(double value, int filled);
void draw_start_position(double shift, double value);
void draw_stop_position(double shift, double value);

/* DISPLAY */
void display();
void reshape();
void display_runners_view();
void display_top_view();
void setViewport(int bottom_left_x, int bottom_left_y, int w, int h);

/* KEYBOARD */
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

/* GAME LOGIC */
void runner_speed_up();
int end_game();
void running();

int main(int argc, char **argv) {
	chosen_runner = 3;
	init_runners();
	set_runners_on_start_position();
	
	view_mode = BEHING_RUNNERS_HEAD;
	game_mode = STOP;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_special);

	end_game();
	running();

	glutMainLoop();
	
	free_runners();
 	return 0;
}

/* DRAWING BOARD */
void draw_board_and_runners() {
	draw_board();

	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> tree_root != NULL) draw_runner(runners[i]);
	}
}

void draw_board() {
	double value = start_position;
	double shift = 5.0;

	
	draw_track(value + NUMBER_OF_RUNERS * shift, ORANGE);
	draw_track(value, GREEN);

	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		draw_track(value, UNFILLED);
		draw_start_position(shift, value);
		value = value + shift;
	}
	draw_stop_position(shift * NUMBER_OF_RUNERS, start_position);
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

void draw_start_position(double shift, double value) {
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);

	glVertex3f(value - start_position, 0, value);
	glVertex3f(value - start_position, 0, value + shift);

	glEnd();
}
void draw_stop_position(double shift, double value) {
	glBegin(GL_LINES);
	glColor3f(1, 0, 1);

	glVertex3f(value - shift, 0, value);
	glVertex3f(value - shift, 0, value + shift);
	glEnd();	
}

/* DISPLAY*/
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

void display_top_view() {
	setViewport(1 * width / 3, 1 * height / 4, width, height);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(-50, 250, -70, -50, 0, -70, 0, 0, 1);
	draw_board_and_runners();

	glPopMatrix();
}

void display_runners_view() {
	setViewport(0, 0, width, height);

	glPushMatrix();
	glLoadIdentity();

	Man* looker = runners[chosen_runner];

	double angle;
	if (looker -> turn_angle == 0) angle = 0;
	else if (looker -> turn_angle == 180.0) angle = -PI;
	else angle = -looker -> turn_angle * PI / 180.0;

	if (view_mode == RUNNERS_EYES) {
		gluLookAt(looker -> head_x, looker -> head_y, looker -> head_z, looker -> head_x + cos(angle), looker -> head_y, looker -> head_z + sin(angle), 0.0,  1.0,  0.0);
	} else {
		double behind_shift = 10.0;
		if (looker -> running_phase == FORWARD) {
			gluLookAt(looker -> head_x - behind_shift, looker -> head_y, looker -> head_z, looker -> head_x + cos(angle), looker -> head_y, looker -> head_z + sin(angle), 0.0,  1.0,  0.0);
		} else if (looker -> running_phase == FORWARD_TURN || looker -> running_phase == BACKWARD_TURN) {
			gluLookAt(looker -> head_x - behind_shift * cos(angle), looker -> head_y, looker -> head_z - behind_shift * sin(angle), looker -> head_x - cos(angle), looker -> head_y, looker -> head_z - sin(angle), 0.0,  1.0,  0.0);
		} else if (looker -> running_phase == BACKWARD) {
			gluLookAt(looker -> head_x + behind_shift, looker -> head_y, looker -> head_z, looker -> head_x + cos(angle), looker -> head_y, looker -> head_z + sin(angle), 0.0,  1.0,  0.0);
		}
	}
	draw_board_and_runners();
	glPopMatrix();
}

void setViewport(int bottom_left_x, int bottom_left_y, int w, int h) {
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	glLoadIdentity();

	glViewport(bottom_left_x, bottom_left_y, w, h);
	gluPerspective(100, 1.0, 1, 300.0);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

/* KEYBOARD */
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27: exit(0); break;
		case 'm':
			if (view_mode == RUNNERS_EYES) view_mode = BEHING_RUNNERS_HEAD;
			else view_mode = RUNNERS_EYES; break;
		case 'n':
			if (game_mode == STOP) game_mode = START; else game_mode = STOP;
			set_runners_on_start_position();
			break;
		case 's': game_mode = START; set_runners_on_start_position(); break;
		case 'p': if (game_mode == PAUSE) game_mode = START; else game_mode = PAUSE; break;
		case '1': if (game_mode == STOP) chosen_runner = 0; break;
		case '2': if (game_mode == STOP) chosen_runner = 1; break;
		case '3': if (game_mode == STOP) chosen_runner = 2; break;
		case '4': if (game_mode == STOP) chosen_runner = 3; break;
		case '5': if (game_mode == STOP) chosen_runner = 4; break;
		case '6': if (game_mode == STOP) chosen_runner = 5; break;
		case '7': if (game_mode == STOP) chosen_runner = 6; break;
		case '8': if (game_mode == STOP) chosen_runner = 7; break;
		
	}
}

void keyboard_special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT: runner_speed_up(); break;
		case GLUT_KEY_RIGHT: runner_speed_up(); printf("%lf %lf %lf \n", runners[chosen_runner] -> head_x, runners[chosen_runner] -> head_y, runners[chosen_runner] -> head_z); break;
		case GLUT_KEY_UP: break;
		case GLUT_KEY_DOWN: break;
	}
}

/* GAME LOGIC */
void runner_speed_up() {
	if (runners[chosen_runner] -> velocity < 1.0) {
		runners[chosen_runner] -> velocity = runners[chosen_runner] -> velocity + 0.01;
	}
	int random_runner = rand() % 8;
	while (random_runner == chosen_runner) random_runner = rand() % 8;

	double random_velocity = 1.0 / ((rand() % 10) * 10.0 + 1.0);
	if (runners[random_runner] -> velocity + random_velocity < 1.0) {
		runners[random_runner] -> velocity = runners[random_runner] -> velocity + random_velocity;
	}
}

void running() {
	end_game();
		printf("game mode %d \n", game_mode);
	if (game_mode == START) {
		walking();
		if (runners[chosen_runner] -> velocity > 0.001) runners[chosen_runner] -> velocity -= 0.001;
		for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
			if (runners[i] -> running_phase == FORWARD) forward(runners[i]);
			else if (runners[i] -> running_phase == FORWARD_TURN) forward_turn(runners[i]);
			else if (runners[i] -> running_phase == BACKWARD) backward(runners[i]);
			else if (runners[i] -> running_phase == BACKWARD_TURN) backward_turn(runners[i]);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(FRAME_ON_MILLISECONDS, running, 0);
}

int end_game() {
	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> head_z > start_position) return END;
	}
	return START;
}