#include "robot.h"

#include <math.h>

ROBOT* create_robot (int port, int type) {
	ROBOT* r = malloc(sizeof(ROBOT));
	
	r->client = playerc_client_create (NULL, "localhost", port);
	if (playerc_client_connect(r->client)) {
		fprintf(stderr, "Error: %s\n", playerc_error_str());
		free(r);
		return NULL;
	}
	
	r->type = type;
	r->state = ACQUIRING_BLOB;
	r->isClosingGripper = 0;
	r->isOpeningGripper = 0;
	
	
	
	if (type == WITH_GRIPPER) {
		r->max_speed = 1.2f;
	}
	else {
		r->max_speed = 2.0f;
	}
	
	r->vlong = r->max_speed;
	r->vrot = 0.4f;
	r->acquired_blob = NULL;
	
	return r;
}

int setup (ROBOT* r) {
	// Position
	r->position2d = playerc_position2d_create(r->client, 0);
	if (playerc_position2d_subscribe(r->position2d, PLAYERC_OPEN_MODE) != 0) {
		fprintf(stderr, "Error: %s\n", playerc_error_str());
		return 0;
	}
	else {
		// Activate
		playerc_position2d_enable(r->position2d, 1);
	}
	
	// Laser
	r->laser = playerc_laser_create(r->client, 0);
	if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE)) {
		fprintf(stderr, "Error: %s\n", playerc_error_str());
		return 0;
	}
	
	// Camera
	r->bf = playerc_blobfinder_create(r->client, 0);
	if (playerc_blobfinder_subscribe(r->bf, PLAYER_OPEN_MODE)) {
		fprintf(stderr, "Error: %s\n", playerc_error_str());
		return 0;
	}
	
	// Gripper
	if (r->type == WITH_GRIPPER) {
		r->gripper = playerc_gripper_create(r->client, 0);
		if (playerc_gripper_subscribe(r->gripper, PLAYER_OPEN_MODE)) {
			fprintf(stderr, "Error: %s\n", playerc_error_str());
			return 0;
		}
		else {
			// Start opened
			open_gripper(r);
		}
	}
	else {
		r->gripper = NULL;
	}
	
	return 1;
}

void read (ROBOT* r) {
	playerc_client_read(r->client);
}

void open_gripper (ROBOT* r) {
	playerc_gripper_open_cmd(r->gripper);
	r->isOpeningGripper = 1;
}

void close_gripper (ROBOT* r) {
	playerc_gripper_close_cmd(r->gripper);
	r->isClosingGripper = 1;
}

int is_gripper_opened (ROBOT* r) {
	return r->gripper->paddles_open;
}

int is_gripper_closed (ROBOT* r) {
	return r->gripper->paddles_closed;
}

void update (ROBOT* r, BLOBLIST* list) {
	if (r->type == 0) {
		// With gripper
		if (r->state == ACQUIRING_BLOB) {
			// Search for blob and set it as acquired
			r->acquired = get_unacquired_blob(list);
			if (r->acquired != NULL) {
				r->state = GOING_NEAR_BLOB;
				acquire(r->acquired);
			}
		}
		else if (r->state == GOING_NEAR_BLOB) {
			// If is near blob, change state to GOING_TO_BLOB
			if (distance(r->position2d->px, r->position2d->py, r->acquired_blob->world_x, r->acquired_blob->world_y) < 8) {
				r->state = GOING_TO_BLOB;
			}
			else {
				// Go to position
				go_to (r, r->acquired_blob->x, r->acquired_blob->y);
			}
		}
		else if (r->state == GOING_TO_BLOB) {
			// Find correct position and go for it
			
			
		}
		else if (r->state == GRABBING_BLOB) {
			// Close gripper if it's not closing already
			if (!r->isClosingGripper) {
				close_gripper(r);
			}
			
		}
		else if (r->state == GOING_TO_BASE) {
			// Go to (0,0)
			go_to (r, 0.f, 0.f);
		}
		else if (r->state == DROPING_BLOB) {
			// Open gripper if it's not opening already
			if (!r->isOpeningGripper) {
				open_gripper(r);
			}
			
			if (is_gripper_opened(r)) {
				r->isOpeningGripper = 0;
				r->state = ADJUST_IN_BASE;
			}
		}
		else if (r->state == ADJUST_IN_BASE) {
			// Rotate until initial rotation
			if (diff(r->position2d->pa, 0) < 5) {
				r->state = ACQUIRING_BLOB;
			}
			else turn_right(r);
		}
	}
}

void turn_left (ROBOT* r) {
	r->vrot = -0.4f;
}

void turn_right (ROBOT* r) {
	r->vrot = 0.4f;
}

void go_to (ROBOT* r, float x, float y) {
	r->dest_x = x;
	r->dest_y = y;
	
	r->vlong = r->max_speed;
}

float diff (float a, float b) {
	float res = a - b;
	
	if (res < 0) return -res;
	else return res;
}

float distance (float x1, float y1, float x2, float y2) {
	return sqrt((x1-x2)*(x1-x2)+ (y1-y2)*(y1-y2)) ;
}

void execute (Robot* r) {
	playerc_position2d_set_cmd_vel(r->position2d, r->vlong, 0, r->vrot, 1);
}

void delete_robot (ROBOT* r) {
	// Disconnect
	playerc_position2d_unsubscribe(r->position2d);
	playerc_position2d_destroy(r->position2d);
	
	playerc_laser_unsubscribe(r->laser);
	playerc_laser_destroy(r->laser);
	
	playerc_blobfinder_unsubscribe(r->bf);
	playerc_blobfinder_destroy(r->bf);
	
	if (r->type == WITH_GRIPPER) {
		playerc_gripper_unsubscribe(r->gripper);
		playerc_gripper_destroy(r->gripper);
	}
	
	playerc_client_disconnect(r->client);
	playerc_client_destroy(r->client);
	
	// Free memory
	free(r);
}