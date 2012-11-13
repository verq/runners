#ifndef MAN_H
#define MAN_H

#define MAX_NUMBER_OF_CHILDREN 	3
#define MAX_NUMBER_OF_BONES 	15
#define MAX_NUMBER_OF_RUNERS	8

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

void init_runers();
void walking();
void draw_runer(Man* runer);

void free_runers();
#endif