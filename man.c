#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "man.h"

/* MAIN FUNCTIONS */
void running();
void init_runners();

/* MOVING PHASES */
void forward(Man* runner);
void forward_turn(Man* runner);
void backward(Man* runner);
void backward_turn(Man* runner);
void change_runner_position(Man* runner, double coord_x, double coord_y, double coord_z, double turn_angle);

/* RUNNERS MOVING */
void walking();
void calculate_angles(int velocity, Bone* root);
void init_tree(Man* runner, double x, double y, double z);
Bone* bone_add_child(Bone* root, double min_angle, double max_angle, double length, double depth);
void swap_min_max(Bone* root);

/* DRAWING */
void draw_runner(Man* runner);
void draw_bone(Bone* root);

/* FREE */
void free_bones(Bone* root);
void free_runners();

/* WRITING TREE */
void write_bone(Bone* b);
void write_tree(Bone* root);


/* MAIN FUNCTIONS */
void init_runners() {
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		runners[i] = (Man*)malloc(sizeof(Man));
		runners[i] -> velocity =  FRAME_EVERY_MILLISECONDS / 17.0;
		runners[i] -> number = i;
		runners[i] -> running_phase = FORWARD;
		runners[i] -> turn_angle = 0.0;
	}
	
	int shift = 5.0;
	int track = 37.0;
	int end_of_track = 37.0;
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		init_tree(runners[i], 15, 8, track);
		runners[i] -> turn_radius = track;
		for (int j = 0; j < PHASES; j++) {
			if (j == BACKWARD || j ==  FORWARD_TURN) runners[i] -> phases[j] = -end_of_track;
			else runners[i] -> phases[j] = end_of_track;
		}
		end_of_track = end_of_track + shift;
		track = track + shift;
	}
}

void running() {
	walking();

	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> running_phase == FORWARD) forward(runners[i]);
		else if (runners[i] -> running_phase == FORWARD_TURN) forward_turn(runners[i]);
		else if (runners[i] -> running_phase == BACKWARD) backward(runners[i]);
		else if (runners[i] -> running_phase == BACKWARD_TURN) backward_turn(runners[i]);
	}

 	glutPostRedisplay();
	glutTimerFunc(FRAME_EVERY_MILLISECONDS, running, 0);
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
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		calculate_angles(runners[i] -> velocity * FRAME_EVERY_MILLISECONDS, runners[i] -> tree_root);
	}
}

void calculate_angles(int velocity, Bone* root) {
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

void init_tree(Man* runner, double x, double y, double z) {
	runner -> head_x = x;
	runner -> head_y = y;
	runner -> head_z = z;
	runner -> head_radius = 0.8;

	runner -> bones[HEAD] = bone_add_child(runner -> tree_root, -90, -90, 0.5, 0.0);

	runner -> bones[HEAD] -> coord_x = x;
	runner -> bones[HEAD] -> coord_y = y;
	runner -> bones[HEAD] -> coord_z = z;

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
	} else if (root -> number_of_children < MAX_NUMBER_OF_CHILDREN) {
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

	for (int i = 0; i < MAX_NUMBER_OF_CHILDREN; i++) {
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

	glColor3f(1, 1, 1);
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
 	glColor3f(1, 1, 1);
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
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		free_bones(runners[i] -> bones[HEAD]);
	}
}

/* WRITING TREE */
void write_bone(Bone* b) {
	printf(" x: %lf y: %lf z: %lf angle: %lf length: %lf children: %d\n", b -> coord_x, b -> coord_y, b -> coord_z, b -> angle, b -> length, b -> number_of_children);
}

void write_tree(Bone* root) {
	if (root == NULL) {
		printf("Tree is empty!\n");
		return;
	} else {
		write_bone(root);
		for (int i = 0; i < root -> number_of_children; i++)
 		write_tree(root -> child[i]);
	}
}

