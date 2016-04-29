#ifndef ROBOT_H
#define ROBOT_H

#include <libplayerc/playerc.h>
#include "bloblist.h"

#define DEBUG(msg) printf("%s - %d - %s\n", msg, __LINE__, __FILE__);

static float hotspots[9][2] =
{
	{ 3, 5 }, { 3, -5 }, { 10, -5 }, { 0, -5 }, { 0, 0 }, { 0, 5 },
	{ -10, 5 }, { -4, 0 }, { -10, -5 }
};
static int n_hotspots = 9;
static int current_hotspot = 0;
static int in_hotspot = 0;
static float current_pa = 0;
static float temp = 0;

enum state {
	ACQUIRING_BLOB,
	GOING_NEAR_BLOB,
	GOING_TO_BLOB,
	LOOKING_AT_BLOB,
	GRABBING_BLOB,
	GOING_TO_BASE,
	DROPPING_BLOB,
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
	int port;
	
	// Config
	playerc_client_t* 		client;
	playerc_position2d_t* 	position2d;
	playerc_laser_t* 		laser;
	playerc_blobfinder_t* 	bf;
	playerc_gripper_t* 		gripper;
	
	// Properties
	float dest_x, dest_y;
	float vlong, vrot;
	
	float max_speed;
	
	struct blob* acquired;
	
} ROBOT;

ROBOT* create_robot (int port, int type);

int setup (ROBOT* r);

void robot_read (ROBOT* r);

void open_gripper (ROBOT* r);

void close_gripper (ROBOT* r);

int is_gripper_opened (ROBOT* r);

int is_gripper_closed (ROBOT* r);

void update (ROBOT* r, BLOBLIST* list);

void turn_left (ROBOT* r);

void turn_right (ROBOT* r);

int go_to (ROBOT* r, float x, float y);

float diff (float a, float b);

void execute (ROBOT* r);

void delete_robot (ROBOT* r);

void update_without_gripper (ROBOT* r, BLOBLIST* list);

void update_with_gripper (ROBOT* r, BLOBLIST* list);

void no_turn (ROBOT* r);

float distance (float x1, float y1, float x2, float y2);

void set_speed (ROBOT* r, float val);

#endif













