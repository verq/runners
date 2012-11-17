#ifndef MAN_L
#define MAN_L

#define PI 			3.1415926535897

#define FRAME_ON_MILLISECONDS 	17.0

#define NUMBER_OF_RUNERS	8

#define STOP			0
#define START			1
#define PAUSE			2
#define END			3

#define FORWARD			0
#define FORWARD_TURN		1
#define BACKWARD		2
#define BACKWARD_TURN		3
#define PHASES			4

#define NUMBER_OF_CHILDREN 	3
#define NUMBER_OF_BONES 	15

typedef struct Bone_t {
	double min_angle, max_angle;
	double coord_x, coord_y, coord_z, depth, angle, length;
	int number_of_children;
	int side;
	struct Bone_t *child[NUMBER_OF_CHILDREN];
	struct Bone_t *parent;
} Bone;

typedef struct Man_t {
	Bone* tree_root;
	double head_x, head_y, head_z, head_radius;
	Bone* bones[NUMBER_OF_BONES];
	double velocity, turn_radius, turn_angle;
	int running_phase, number;
	double phases[PHASES];
} Man;

Man* runners[NUMBER_OF_RUNERS];
int game_mode;
int chosen_runner;
double start_shift;

void init_runners();
void set_runners_on_start_position();

void forward(Man* runner);
void forward_turn(Man* runner);
void backward(Man* runner);
void backward_turn(Man* runner);
void walking();

void draw_runner(Man* runner);

void free_runners();



#endif