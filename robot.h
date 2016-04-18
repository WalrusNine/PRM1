#ifndef ROBOT_H
#define ROBOT_H

enum state {
	ACQUIRING_BLOB,
	GOING_NEAR_BLOB,
	GOING_TO_BLOB,
	GRABBING_BLOB,
	GOING_TO_BASE,
	DROPING_BLOB,
	ADJUST_IN_BASE
};

enum type {
	WITH_GRIPPER,
	WITHOUT_GRIPPER
};

typedef struct robot {
	// State
	int state;
	int isClosingGripper;
	int isOpeningGripper;
	int type;
	
	// Config
	playerc_client_t* 		client;
	playerc_position2d_t* 	position2d;
	playerc_laser_t* 			laser;
	playerc_blobfinder_t* 	bf;
	playerc_gripper_t* 		gripper;
	
	// Properties
	float dest_x, dest_y;
	float vlong, vrot;
	
	float max_speed;
	
	BLOB* acquired_blob;
	
} ROBOT;

int create_robot (int port, int type);

int setup (ROBOT* r);

void read (ROBOT* r);

void open_gripper (ROBOT* r);

void close_gripper (ROBOT* r);

int is_gripper_opened (ROBOT* r);

int is_gripper_closed (ROBOT* r);

void update (ROBOT* r);

void turn_left (ROBOT* r);

void turn_right (ROBOT* r);

void go_to (ROBOT* r, float x, float y);

float diff (float a, float b);

void execute (Robot* r);

void delete_robot (ROBOT* r);

#endif













