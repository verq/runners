#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>

#define PI 			3.1415926535897
#define MAX_NUMBER_OF_CHILDREN 	3
#define MAX_NUMBER_OF_FRAMES 	8.0
#define MAX_NUMBER_OF_BONES 	15
#define MAX_NUMBER_OF_RUNERS	8

#define STOP 		0
#define START 		1
#define ON_KEY 		2

#define HEAD 		0
#define BACK 		1
#define LEG_LEFT 	2
#define ANKLE_LEFT	3
#define FOOT_LEFT	4
#define TOES_LEFT	5
#define LEG_RIGHT	6
#define ANKLE_RIGHT	7
#define FOOT_RIGHT	8
#define TOES_RIGHT	9
#define ARM_LEFT	10
#define FOREARM_LEFT	11
#define ARM_RIGHT	12
#define FOREARM_RIGHT	13

typedef struct Bone_t {
	double max_angle, min_angle;
	double coord_x, coord_y, angle, length, depth;
	int number_of_children;
	int side;
	struct Bone_t *child[MAX_NUMBER_OF_CHILDREN];
	struct Bone_t *parent;
} Bone;

typedef struct Man_t {
	Bone* tree_root;
	double head_x, head_y, head_z, head_radius;
	Bone* bones[MAX_NUMBER_OF_BONES];
	int velocity;
} Man;

Man* runers[MAX_NUMBER_OF_RUNERS];

int animation;


void walking();
void init_runers();
void draw_runer(Man* runer);

void init_tree(Man* runer, double x, double y, double z);
Bone* bone_add_child(Bone* root, double x, double y, double z, double a, double l, double d);
void swap_min_max(Bone* root);
void free_bones(Bone* root);
void free_runers();

void draw_bone(Bone* root);
void draw_circle(double x, double y, double z, double radius);
void calculate_angles(int velocity, Bone* root);

void free_bones(Bone* root);
void free_runers();

void write_bone(Bone* b);
void write_tree(Bone* root);

void init_runers() {
	animation = START;
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		runers[i] = (Man*)malloc(sizeof(Man));
		runers[i] -> velocity = MAX_NUMBER_OF_FRAMES;
	}
	int shift = 5;
	int track = 30;
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		init_tree(runers[i], 0, 0, track);
		track = track + shift;
	}
}

void walking() {
	if (animation == STOP) return;
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		calculate_angles(runers[i] -> velocity, runers[i] -> tree_root);
		printf("%d \t", i);
		write_bone(runers[i] -> bones[HEAD]);
		write_bone(runers[i] -> bones[HEAD]);
	}
	glutPostRedisplay();
	if (animation != ON_KEY) glutTimerFunc(100, walking, 0);
}

void init_tree(Man* runer, double x, double y, double z) {
	runer -> head_x = x;
	runer -> head_y = y;
	runer -> head_z = z;
	runer -> head_radius = 0.8;

	runer -> bones[HEAD] = bone_add_child(runer -> tree_root, x, y, 0, -90, 0.5, 0.0);
	runer -> bones[HEAD] -> max_angle = -90;

	runer -> bones[BACK] = bone_add_child(runer -> bones[HEAD], 0.0, 0.0, 0, 0, 3, 0.0);
	runer -> bones[BACK] -> max_angle = 0;

	runer -> bones[LEG_LEFT] = bone_add_child(runer -> bones[BACK], 0.0, 0.0, 0, 30, 2, 0.01);
	runer -> bones[LEG_LEFT] -> max_angle = -15;
	runer -> bones[ANKLE_LEFT] = bone_add_child(runer -> bones[LEG_LEFT], 0.0, 0.0, 0, 0, 2, 0.0);
	runer -> bones[ANKLE_LEFT] -> max_angle = -30;
	runer -> bones[FOOT_LEFT] = bone_add_child(runer -> bones[ANKLE_LEFT], 0.0, 0.0, 0, 90, 0.5, 0.0);
	runer -> bones[FOOT_LEFT] -> max_angle = 90;
	runer -> bones[TOES_LEFT] = bone_add_child(runer -> bones[FOOT_LEFT], 0.0, 0.0, 0, 0, 0.1, 0.0);
	runer -> bones[TOES_LEFT] -> max_angle = 45;

	runer -> bones[LEG_RIGHT] = bone_add_child(runer -> bones[BACK], 0.0, 0.0, 0, -15, 2, -0.01);
	runer -> bones[LEG_RIGHT] -> max_angle = 30;
	runer -> bones[ANKLE_RIGHT] = bone_add_child(runer -> bones[LEG_RIGHT], 0.0, 0.0, 0, -30, 2, 0.0);
	runer -> bones[ANKLE_RIGHT] -> max_angle = 0;
	runer -> bones[FOOT_RIGHT] = bone_add_child(runer -> bones[ANKLE_RIGHT], 0.0, 0.0, 0, 90, 0.5, 0.0);
	runer -> bones[FOOT_RIGHT] -> max_angle = 90;
	runer -> bones[TOES_RIGHT] = bone_add_child(runer -> bones[FOOT_RIGHT], 0.0, 0.0, 0, 45, 0.1, 0.0);
	runer -> bones[TOES_RIGHT] -> max_angle = 0;

	runer -> bones[ARM_LEFT] = bone_add_child(runer -> bones[HEAD], 0.0, 0.0, 0, -45, 1.5, 0.01);
	runer -> bones[ARM_LEFT] -> max_angle = 45;
	runer -> bones[FOREARM_LEFT] = bone_add_child(runer -> bones[ARM_LEFT], 0.0, 0.0, 0, 100, 1.5, 0.0);
	runer -> bones[FOREARM_LEFT] -> max_angle = 100;

	runer -> bones[ARM_RIGHT] = bone_add_child(runer -> bones[HEAD], 0.0, 0.0, 0, 45, 1.5, -0.01);
	runer -> bones[ARM_RIGHT] -> max_angle = -45;
	runer -> bones[FOREARM_RIGHT] = bone_add_child(runer -> bones[ARM_RIGHT], 0.0, 0.0, 0, 100, 1.5, 0.0);
	runer -> bones[FOREARM_RIGHT] -> max_angle = 100;

	swap_min_max(runer -> bones[HEAD]);
	runer -> tree_root = runer -> bones[HEAD];
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

Bone* bone_add_child(Bone* root, double x, double y, double z, double a, double l, double d) {
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
	root -> coord_x = x;
	root -> coord_y = y;
	root -> angle = a;
	root -> min_angle = a;
	root -> length = l;
	root -> number_of_children = 0;
	root -> side = 0;
	root -> depth = d;

	for (int i = 0; i < MAX_NUMBER_OF_CHILDREN; i++) {
		root -> child[i] = NULL;
	}
	return root;
}

void draw_runer(Man* runer) {
	draw_circle(runer -> head_x, runer -> head_y, 0, runer -> head_radius);
	draw_bone(runer -> tree_root);
}

void draw_bone(Bone* root) {
	glPushMatrix();

	glTranslatef(root -> coord_x, root -> coord_y, 0);
	glRotatef(root -> angle, 0.0, 0.0, -1.0);

	glBegin(GL_LINES);
 	glColor3f(1, 1, 1);
 	glVertex3f(0, 0, 0); //TODO, bo trzeba zaczynac w odpowiedniej glebokosci
 	glColor3f(1, 1, 1);
 	glVertex3f(root -> length, 0, root -> depth); //TODO
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
		glVertex2f(real_x + x, real_y + y); //TODO

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

void free_runers() {
	for (int i = 0; i < MAX_NUMBER_OF_RUNERS; i++) {
		free_bones(runers[i] -> bones[HEAD]);
	}
}

/* WRITING TREE */
void write_bone(Bone* b) {
	printf(" x: %lf y: %lf  angle: %lf length: %lf children: %d\n", b -> coord_x, b -> coord_y, b -> angle, b -> length, b -> number_of_children);
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

