#ifndef MAN_H
#define MAN_H

#define MAX_NUMBER_OF_CHILDREN 	3
#define MAX_NUMBER_OF_BONES 	15
#define MAX_NUMBER_OF_RUNERS	8
#define POSITIONS		4

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
	int running_phase;
	int number;
	double positions[POSITIONS];
} Man;

Man* runners[MAX_NUMBER_OF_RUNERS];

void init_runners();
void walking();
void running();
void draw_runner(Man* runner);

void free_runners();
#endif