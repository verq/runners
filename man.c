#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "man.h"

#define HEAD 			0
#define BACK 			1
#define LEG_LEFT 		2
#define ANKLE_LEFT		3
#define FOOT_LEFT		4
#define TOES_LEFT		5
#define LEG_RIGHT		6
#define ANKLE_RIGHT		7
#define FOOT_RIGHT		8
#define TOES_RIGHT		9
#define ARM_LEFT		10
#define FOREARM_LEFT		11
#define ARM_RIGHT		12
#define FOREARM_RIGHT		13

/* MAIN FUNCTIONS */
void set_runners_on_start_position();
void init_runners();

/* MOVING PHASES */
void forward(Man* runner);
void forward_turn(Man* runner);
void backward(Man* runner);
void backward_turn(Man* runner);
void change_runner_position(Man* runner, double coord_x, double coord_y, double coord_z, double turn_angle);

/* RUNNERS MOVING */
void walking();
void calculate_angles(double velocity, Bone* root);
void init_tree(Man* runner);
Bone* bone_add_child(Bone* root, double min_angle, double max_angle, double length, double depth);
void swap_min_max(Bone* root);

/* DRAWING */
void draw_runner(Man* runner);
void draw_bone(Bone* root);

/* FREE */
void free_bones(Bone* root);
void free_runners();


/* MAIN FUNCTIONS */
void set_runners_on_start_position() {
	start_shift = 12.0;
	if (runners == NULL) init_runners();
	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		
		runners[i] -> velocity = 1.0 / (rand() % 50 + 1);
		runners[i] -> number = i;
		runners[i] -> running_phase = FORWARD;
		runners[i] -> turn_angle = 0.0;
	}

	int shift = 5.0;
	int track = 37.0;
	int end_of_track = 37.0;
	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		change_runner_position(runners[i], start_shift * i - shift + 1.0, 8.0, track, 0.0);
		runners[i] -> turn_radius = track;
		for (int j = 0; j < PHASES; j++) {
			if (j == BACKWARD || j ==  FORWARD_TURN) runners[i] -> phases[j] = -end_of_track;
			else runners[i] -> phases[j] = end_of_track;
		}
		end_of_track = end_of_track + shift;
		track = track + shift;
	}
}

void init_runners() {
	srand(time(NULL));
	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		runners[i] = (Man*)malloc(sizeof(Man));
	}

	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		init_tree(runners[i]);
	}
}

/* MOVING PHASES */
void forward(Man* runner) {
	runner -> turn_angle = 0.0;
	if (runner -> head_x < runner -> phases[FORWARD]) {
		change_runner_position(runner, runner -> head_x + runner -> velocity, runner -> head_y, runner -> head_z, runner -> turn_angle);
	} else {
		runner -> running_phase = FORWARD_TURN;
		forward_turn(runner);
	}
}

void backward(Man* runner) {
	runner -> turn_angle = 180.0;
	if (runner -> head_x > runner -> phases[BACKWARD]) {
		change_runner_position(runner, runner -> head_x - runner -> velocity, runner -> head_y, runner -> head_z, runner -> turn_angle);
	} else {
		runner -> running_phase = BACKWARD_TURN;
		backward_turn(runner);
	}
}

void forward_turn(Man* runner) {
	if (runner -> head_z >= runner -> phases[FORWARD_TURN] + 0.1) {
		double angle = asin((runner -> head_x - runner -> turn_radius) / runner -> turn_radius);
		
		if (runner -> head_z < 0) angle = PI - angle;
		double x = runner -> turn_radius * sin(angle + runner -> velocity / runner -> turn_radius) + runner -> turn_radius;
		double z = runner -> turn_radius * cos(angle + runner -> velocity / runner -> turn_radius);
		
		change_runner_position(runner, x, runner -> head_y, z, 180.0 * angle / PI);
	} else {
		runner -> running_phase = BACKWARD;
		backward(runner);
	}
}

void backward_turn(Man* runner) {
	if (runner -> head_z < runner -> phases[BACKWARD_TURN] - 0.1) {
		double angle = asin((runner -> head_x + runner -> turn_radius) / runner -> turn_radius);

		if (runner -> head_z < 0) angle = PI - angle;
		double z = runner -> turn_radius * cos(angle + runner -> velocity / runner -> turn_radius);
		double x = runner -> turn_radius * sin(angle + runner -> velocity / runner -> turn_radius) - runner -> turn_radius;

		change_runner_position(runner, x, runner -> head_y, z, 180.0 * angle/PI);
	} else {
		runner -> running_phase = FORWARD;
		forward(runner);
	}
}

void change_runner_position(Man* runner, double coord_x, double coord_y, double coord_z, double turn_angle) {
	runner -> head_x = coord_x;
	runner -> head_y = coord_y;
	runner -> head_z = coord_z;

	runner -> bones[HEAD] -> coord_x = coord_x;
	runner -> bones[HEAD] -> coord_y = coord_y;
	runner -> bones[HEAD] -> coord_z = coord_z;

	runner -> turn_angle = turn_angle;
}

/* RUNNERS MOVES */

void walking() {
	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		calculate_angles(FRAME_ON_MILLISECONDS / runners[i] -> velocity, runners[i] -> tree_root);
	}
}

void calculate_angles(double velocity, Bone* root) {
	if (root -> max_angle - root -> min_angle != 0) {

		double temp = (abs(root -> max_angle) + abs(root -> min_angle)) / velocity;
		if (root -> side == 0) {
			if (root -> angle + temp < root -> max_angle) {
				root -> angle = root -> angle + temp;
			} else {
				root -> angle = root -> angle - temp;
				root -> side = 1;
			}
		} else {
			if (root -> angle - temp > root -> min_angle) {
				root -> angle = root -> angle - temp;
			} else {
				root -> angle = root -> angle + temp;
				root -> side = 0;
			}
		}
	}
	for (int i = 0; i < root -> number_of_children; i++) {
		calculate_angles(velocity, root -> child[i]);
	}
}

void init_tree(Man* runner) {
	runner -> head_radius = 0.8;

	runner -> bones[HEAD] = bone_add_child(runner -> tree_root, -90, -90, 0.5, 0.0);
	runner -> bones[BACK] = bone_add_child(runner -> bones[HEAD], 0, 0, 3.0, 0.0);

	runner -> bones[LEG_LEFT] = bone_add_child(runner -> bones[BACK], 30, -15, 2.0, -0.5);
	runner -> bones[ANKLE_LEFT] = bone_add_child(runner -> bones[LEG_LEFT], 0, -30, 2.0, -0.1);
	runner -> bones[FOOT_LEFT] = bone_add_child(runner -> bones[ANKLE_LEFT], 90, 90, 0.5, 0.0);
	runner -> bones[TOES_LEFT] = bone_add_child(runner -> bones[FOOT_LEFT], 0, 45, 0.1, 0.0);

	runner -> bones[LEG_RIGHT] = bone_add_child(runner -> bones[BACK], -15, 30, 2.0, 0.5);
	runner -> bones[ANKLE_RIGHT] = bone_add_child(runner -> bones[LEG_RIGHT], -30, 0, 2.0, 0.1);
	runner -> bones[FOOT_RIGHT] = bone_add_child(runner -> bones[ANKLE_RIGHT], 90, 90, 0.5, 0.0);
	runner -> bones[TOES_RIGHT] = bone_add_child(runner -> bones[FOOT_RIGHT], 45, 0, 0.1, 0.0);

	runner -> bones[ARM_LEFT] = bone_add_child(runner -> bones[HEAD], -45, 45, 1.5, -0.5);
	runner -> bones[FOREARM_LEFT] = bone_add_child(runner -> bones[ARM_LEFT], 100, 100, 1.5, -0.5);

	runner -> bones[ARM_RIGHT] = bone_add_child(runner -> bones[HEAD], 45, -45, 1.5, 0.5);
	runner -> bones[FOREARM_RIGHT] = bone_add_child(runner -> bones[ARM_RIGHT], 100, 100, 1.5, 0.5);

	runner -> tree_root = runner -> bones[HEAD];
	
}

Bone* bone_add_child(Bone* root, double min_angle, double max_angle, double length, double depth) {
	if (root == NULL) {
		root = (Bone*)malloc(sizeof(Bone));
		root -> parent = NULL;
	} else if (root -> number_of_children < NUMBER_OF_CHILDREN) {
		Bone* temp;
		temp = (Bone*)malloc(sizeof(Bone));
		temp -> parent = root;
		root -> child[root -> number_of_children++] = temp;
		root = temp;
	} else {
		return NULL;
	}
	root -> coord_x = 0.0;
	root -> coord_y = 0.0; 
	root -> coord_z = 0.0;
	root -> depth = depth;
	
	root -> angle = min_angle;
	root -> max_angle = max_angle;
	root -> min_angle = min_angle;

	root -> length = length;
	root -> number_of_children = 0;
	root -> side = 0;
	
	swap_min_max(root);

	for (int i = 0; i < NUMBER_OF_CHILDREN; i++) {
		root -> child[i] = NULL;
	}
	return root;
}

void swap_min_max(Bone* root) {
	if (root -> min_angle > root -> max_angle) {
		double temp = root -> min_angle;
		root -> min_angle = root -> max_angle;
		root -> max_angle = temp;
		root -> side = 1;
	} else {
		root -> side = 0;
	}
}

/* DRAWING */

void draw_runner(Man* runner) {
	glPushMatrix();
	
	glTranslatef(runner -> head_x, 0.0, runner -> head_z);
	glRotatef(runner -> turn_angle, 0.0, 1.0, 0.0);
	glTranslatef(0.0, runner -> head_y + runner -> head_radius, 0.0);

	if (runner -> number == chosen_runner) glColor3f(0, 1, 0);
	else glColor3f(1, 1, 1);

	glutSolidSphere(runner -> head_radius, 100, 100);
	glTranslatef(-runner -> head_x, -runner -> head_y - runner -> head_radius , -runner -> head_z);
	
	draw_bone(runner -> tree_root);
	glPopMatrix();

}

void draw_bone(Bone* root) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glTranslatef(root -> coord_x, root -> coord_y, root -> coord_z);
	glRotatef(root -> angle, 0.0, 0.0, 1.0);

	glBegin(GL_LINES);
 	glVertex3f(0, 0, 0);
	glVertex3f(root -> length, 0.0, root -> depth);
	glEnd();

 	glTranslatef(root -> length, 0.0, root -> depth);
 	for (int i = 0; i < root -> number_of_children; i++) {
 		draw_bone(root -> child[i]);
	}
 	glPopMatrix();
}

/* FREE */
void free_bones(Bone* root) {
	if (root == NULL) return;
	else {
		for (int i = 0; i < root -> number_of_children; i++) {
			free_bones(root -> child[i]);
		}
		free(root);
	}
}

void free_runners() {
	for (int i = 0; i < NUMBER_OF_RUNERS; i++) {
		free_bones(runners[i] -> bones[HEAD]);
	}
}