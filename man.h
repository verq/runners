#ifndef MAN_H
#define MAN_H

#define PI 			3.1415926535897

#define MAX_NUMBER_OF_CHILDREN 	3
#define FRAME_EVERY_MILLISECONDS 17.0
#define MAX_NUMBER_OF_BONES 	15
#define MAX_NUMBER_OF_RUNERS	8

#define STOP			0
#define START			1
#define PAUSE			2

#define FORWARD			0
#define FORWARD_TURN		1
#define BACKWARD		2
#define BACKWARD_TURN		3
#define PHASES			4

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

typedef struct Bone_t {
	double min_angle, max_angle;
	double coord_x, coord_y, coord_z, depth, angle, length;
	int number_of_children;
	int side;
	struct Bone_t *child[MAX_NUMBER_OF_CHILDREN];
	struct Bone_t *parent;
} Bone;

typedef struct Man_t {
	Bone* tree_root;
	double head_x, head_y, head_z, head_radius;
	Bone* bones[MAX_NUMBER_OF_BONES];
	double velocity, turn_radius, turn_angle;
	int running_phase, number;
	double phases[PHASES];
} Man;

Man* runners[MAX_NUMBER_OF_RUNERS];
int game_mode;

void init_runners();
void start_runners();
void running();
void draw_runner(Man* runner);

void free_runners();

#endif