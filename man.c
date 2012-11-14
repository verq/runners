#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "man.h"

Man* runners[MAX_NUMBER_OF_RUNERS];

int animation;

void running();
void walking();
void init_runners();
void draw_runner(Man* runner);

void forward(Man* runner);
void forward_turn(Man* runner);
void backward(Man* runner);
void backward_turn(Man* runner);

void init_tree(Man* runner, double x, double y, double z);
Bone* bone_add_child(Bone* root, double min_angle, double max_angle, double length);
void swap_min_max(Bone* root);
void free_bones(Bone* root);
void free_runners();

void draw_bone(Bone* root);
void draw_circle(double x, double y, double z, double radius);
void calculate_angles(int velocity, Bone* root);

void free_bones(Bone* root);
void free_runners();

void write_bone(Bone* b);
void write_tree(Bone* root);

void init_runners() {
	animation = START;
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		runners[i] = (Man*)malloc(sizeof(Man));
		runners[i] -> velocity = 5.0;
		runners[i] -> number = i;
		runners[i] -> running_phase = FORWARD;
		runners[i] -> turn = 0;
	}
	
	int shift = 5.0;
	int track = 37.0;
	int end_of_track = 35.0;
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		init_tree(runners[i], 15, 8, track);
		for (int j = 0; j < PHASES; j++) {
			if (j == BACKWARD || j ==  FORWARD_TURN) runners[i] -> phases[j] = -end_of_track;
			else runners[i] -> phases[j] = end_of_track;
		}
		end_of_track = end_of_track + shift;
		track = track + shift;
	}
}

void running() {
	if (animation == STOP) return;
	walking();

	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		if (runners[i] -> running_phase == FORWARD) forward(runners[i]);
		else if (runners[i] -> running_phase == FORWARD_TURN) forward_turn(runners[i]);
		else if (runners[i] -> running_phase == BACKWARD) backward(runners[i]);
		else if (runners[i] -> running_phase == BACKWARD_TURN) backward_turn(runners[i]);
	}

	glutPostRedisplay();
	glutTimerFunc(MAX_NUMBER_OF_FRAMES, running, 0);
}

void forward(Man* runner) {
	if (runner -> head_x < runner -> phases[FORWARD]) {
		runner -> bones[HEAD] -> coord_x += runner -> velocity;
		runner -> head_x += runner -> velocity;
	} else {
		runner -> running_phase = FORWARD_TURN;
		runner -> turn = 0;
		forward_turn(runner);
	}
}

void backward(Man* runner) {
	if (runner -> head_x > runner -> phases[BACKWARD]) {
		runner -> bones[HEAD] -> coord_x -= runner -> velocity;
		runner -> head_x -= runner -> velocity;
	} else {
		runner -> running_phase = BACKWARD_TURN;
		backward_turn(runner);
	}
}

void forward_turn(Man* runner) {
	if (runner -> head_z > runner -> phases[FORWARD_TURN]) {
		double preangle = (runner -> head_x - runner -> phases[FORWARD]) / runner -> phases[FORWARD] ;
		double angle = asin(preangle);
		
		double l = 1.0;
		double g = 60.0;

		if (runner -> head_z < 0) angle = PI - angle;
		double z = runner -> phases[FORWARD] * cos(angle + l*PI/g) ;
		double x = runner -> phases[FORWARD] * sin(angle + l*PI/g) + runner -> phases[FORWARD];
			
		runner -> turn += runner -> velocity;
		
		runner -> head_x = x;
		runner -> head_z = z;
		runner -> bones[HEAD] -> coord_x = x;
		runner -> bones[HEAD] -> coord_z = z;

	} else {
		runner -> running_phase = BACKWARD;
		backward(runner);
	}
}

void backward_turn(Man* runner) {
	if (runner -> head_z < runner -> phases[BACKWARD_TURN]) {
		runner -> bones[HEAD] -> coord_z += runner -> velocity;
		runner -> head_z += runner -> velocity;
	} else {
		runner -> running_phase = FORWARD;
		forward(runner);
	}
}

void walking() {
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		calculate_angles(runners[i] -> velocity, runners[i] -> tree_root);
	}
}

void init_tree(Man* runner, double x, double y, double z) {
	runner -> head_x = x;
	runner -> head_y = y;
	runner -> head_z = z;
	runner -> head_radius = 0.8;

	runner -> bones[HEAD] = bone_add_child(runner -> tree_root, -90, -90, 0.5);
	runner -> bones[HEAD] -> max_angle = -90;

	runner -> bones[HEAD] -> coord_x = x;
	runner -> bones[HEAD] -> coord_y = y;
	runner -> bones[HEAD] -> coord_z = z;

	runner -> bones[BACK] = bone_add_child(runner -> bones[HEAD], 0, 0, 3);

	runner -> bones[LEG_LEFT] = bone_add_child(runner -> bones[BACK], 30, -15, 2);
	runner -> bones[ANKLE_LEFT] = bone_add_child(runner -> bones[LEG_LEFT], 0, -30, 2);
	runner -> bones[FOOT_LEFT] = bone_add_child(runner -> bones[ANKLE_LEFT], 90, 90, 0.5);
	runner -> bones[TOES_LEFT] = bone_add_child(runner -> bones[FOOT_LEFT], 0, 45, 0.1);

	runner -> bones[LEG_RIGHT] = bone_add_child(runner -> bones[BACK], -15, 30, 2);
	runner -> bones[ANKLE_RIGHT] = bone_add_child(runner -> bones[LEG_RIGHT], -30, 0, 2);
	runner -> bones[FOOT_RIGHT] = bone_add_child(runner -> bones[ANKLE_RIGHT], 90, 90, 0.5);
	runner -> bones[TOES_RIGHT] = bone_add_child(runner -> bones[FOOT_RIGHT], 45, 0, 0.1);

	runner -> bones[ARM_LEFT] = bone_add_child(runner -> bones[HEAD], -45, 45, 1.5);
	runner -> bones[FOREARM_LEFT] = bone_add_child(runner -> bones[ARM_LEFT], 100, 100, 1.5);

	runner -> bones[ARM_RIGHT] = bone_add_child(runner -> bones[HEAD], 45, -45, 1.5);
	runner -> bones[FOREARM_RIGHT] = bone_add_child(runner -> bones[ARM_RIGHT], 100, 100, 1.5);

	swap_min_max(runner -> bones[HEAD]);
	runner -> tree_root = runner -> bones[HEAD];
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

Bone* bone_add_child(Bone* root, double min_angle, double max_angle, double length) {
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
	root -> angle = min_angle;
	root -> max_angle = max_angle;
	root -> min_angle = min_angle;
	root -> length = length;
	root -> number_of_children = 0;
	root -> side = 0;

	for (int i = 0; i < MAX_NUMBER_OF_CHILDREN; i++) {
		root -> child[i] = NULL;
	}
	return root;
}

void draw_runner(Man* runner) {
	draw_circle(runner -> head_x, runner -> head_y, runner -> head_z, runner -> head_radius);
	draw_bone(runner -> tree_root);
}

void draw_bone(Bone* root) {
	glPushMatrix();

	glTranslatef(root -> coord_x, root -> coord_y, root -> coord_z); //TODO
	glRotatef(root -> angle, 0.0, 0.0, 1.0);

	glBegin(GL_LINES);
 	glColor3f(1, 1, 1);
 	glVertex3f(0, 0, 0); //TODO, bo trzeba zaczynac w odpowiedniej glebokosci
 	glColor3f(1, 1, 1);
 	glVertex3f(root -> length, 0, 0); //TODO
	glEnd();

 	glTranslatef(root -> length, 0.0, 0.0);

 	for (int i = 0; i < root -> number_of_children; i++) {
 		draw_bone(root -> child[i]);
	}
 	glPopMatrix();

}

void draw_circle(double x, double y, double z, double radius) {
	glBegin(GL_LINE_LOOP);
	int number_of_segments = 100;
	for(int i = 0; i < number_of_segments; i++) {
		double angle = 2.0 * PI * i / number_of_segments;
		double real_x = radius * cos(angle);
		double real_y = radius * sin(angle);

		glColor3f(1, 1, 1);
		glVertex3f(real_x + x, real_y + y, z); //TODO
	}
	glEnd();
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
	for (int i = 0; i < root -> number_of_children; i++) {
		swap_min_max(root -> child[i]);
	}
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

