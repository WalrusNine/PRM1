#ifndef ROBOT_H
#define ROBOT_H

#include <libplayerc/playerc.h>
#include "bloblist.h"

#define DEBUG(msg) printf("%s - %d - %s\n", msg, __LINE__, __FILE__);

// Turning points for the camera
static float hotspots[9][2] =
{
	{ 3, 5 }, { 3, -5 }, { 3, 0 }, { 13, 5 }, { 13, 0 }, { 13, -5 },
	{ 23, 5 }, { 23, 0 }, { 23, -5 }
};
static int n_hotspots = 9;
static int current_hotspot = 0;

// To know whether it's in a hotspot or not, if should turn or keep going
static int in_hotspot = 0;

// TODO: Stop turning
static float current_pa = 0;
static float temp = 0;

// Whether there's a robot going for a blob or not
static int is_robot_acting = 0;

enum state {
	ACQUIRING_BLOB,		// Is looking at list for an unacquired blob
	GOING_NEAR_BLOB,	// Is going to blob based on cotoco's estimation
	GOING_TO_BLOB,		// Uses own camera to get precise position
	LOOKING_AT_BLOB,	// Temporary, not in use
	GRABBING_BLOB,		// Stop and close gripper
	GOING_TO_BASE,		// Going back to 0,0
	DROPPING_BLOB,		// Stop and open gripper
	ADJUST_IN_BASE		// Go back to initial position
};

enum type {
	WITH_GRIPPER,
	WITHOUT_GRIPPER
};

typedef struct robot {
	// State
	int state;
	
	// Gripper state variables
	int isClosingGripper;
	int isOpeningGripper;
	
	int type;
	
	// 6665, 6666, 6667
	int port;
	
	// Config
	playerc_client_t* 		client;
	playerc_position2d_t* 	position2d;
	playerc_laser_t* 		laser;
	playerc_blobfinder_t* 	bf;			// Camera
	playerc_gripper_t* 		gripper;
	
	// Properties
	float dest_x, dest_y;	// Destination
	float vlong, vrot;
	
	float max_speed;
	
	struct blob* acquired_blob;	// The acquired blob
	
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

void capture_blobs (ROBOT* r, BLOBLIST* list);

#endif













