#include "robot.h"
#include "bloblist.h"

#include <math.h>

ROBOT* create_robot (int port, int type) {
	ROBOT* r = malloc(sizeof(ROBOT));
	
	r->client = playerc_client_create (NULL, "localhost", port);
	if (playerc_client_connect(r->client) != 0) {
		fprintf(stderr, "Error: %s\n", playerc_error_str());
		free(r);
		return NULL;
	}
	
	r->port = port;				// Identify the robot
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
	
	r->vlong = 0;
	r->vrot = 0;
	r->acquired_blob = NULL;		// No blob acquired yet

	// Setup initial pos
	r->initial_x = 0;
	if ((port % 10) == 5) {
		r->initial_y = -1;
	}
	else if ((port % 10) == 6) {
		r->initial_y = 0;
	}
	else if ((port % 10) == 7) {
		r->initial_y = 1;
	}

	setup(r);

	robot_read(r);
	robot_read(r);
	robot_read(r);
	robot_read(r);
	
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
		if (r->type == WITHOUT_GRIPPER) {
			current_pa = r->position2d->pa;
		}
	}
	
	// Laser
	r->laser = playerc_laser_create(r->client, 0);
	if (playerc_laser_subscribe(r->laser, PLAYERC_OPEN_MODE)) {
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

void robot_read (ROBOT* r) {
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
	return r->gripper->state == PLAYER_GRIPPER_STATE_OPEN;
}

int is_gripper_closed (ROBOT* r) {
	return r->gripper->state == PLAYER_GRIPPER_STATE_CLOSED;
}

void update (ROBOT* r, BLOBLIST* list) {
	if (r->type == WITH_GRIPPER) {
		update_with_gripper(r, list);
	}
	else update_without_gripper(r, list);
}

void update_without_gripper (ROBOT* r, BLOBLIST* list) {
	if (!in_hotspot) {
		// Update current pa - temporary, test
		current_pa = r->position2d->pa;
		
		// Get all blobs in camera, add to list
		capture_blobs(r, list);
		
		// Go to next hotspot
		int rsc = go_to(r, hotspots[current_hotspot][0], hotspots[current_hotspot][1]);
		float dist = distance(r->position2d->px, r->position2d->py, hotspots[current_hotspot][0], hotspots[current_hotspot][1]);
		
		if (dist < 1.2f) {
			// Reached
			in_hotspot = 1;
			r->vlong = 0;
		}
	}
	else {
		// Get all blobs in camera, add to list
		capture_blobs(r, list);

		// Reached, turn right until goes around, then change hotspot
		turn_right(r);
		r->vlong = 0;
		
		temp += 0.4f;
		if (temp >= 18) {
			in_hotspot = 0;
			current_hotspot++;
		}
	}
}

void capture_blobs (ROBOT* r, BLOBLIST* list) {
	// Get all blobs in camera, add to list
	int k;
	for(k = 0; k < r->bf->blobs_count; k++){
		if (r->bf->blobs[k].color == 65280){
			// Found blob
			int blob_x = r->bf->blobs[k].x;
			float blob_range = r->bf->blobs[k].range;

			// Get angle to blob
			int temp_diff = 40 - blob_x;
			float angle = temp_diff;
			
			angle *= (3.f / 4.f);
			angle *= (M_PI / 180.f); // Radians

			float dest_x = r->position2d->px + blob_range * cos(angle);
			float dest_y = r->position2d->py + blob_range * sin(angle);

			add_node(list, create_node(create_blob(dest_x, dest_y, r->bf->blobs[k].id)));
		}
	}
}

void update_with_gripper (ROBOT* r, BLOBLIST* list) {
	// With gripper
	if (r->state == ACQUIRING_BLOB) {
		// Search for blob and set it as acquired
		r->acquired_blob = get_unacquired_blob(list);
		if (r->acquired_blob != NULL && !is_robot_acting) {
			r->state = GOING_NEAR_BLOB;
			acquire(r->acquired_blob);
			is_robot_acting = 1;
		}
	}
	else if (r->state == GOING_NEAR_BLOB) {
		// If is near blob, change state to GOING_TO_BLOB
		// Go to position
		int is_near = go_to (r, r->acquired_blob->x, r->acquired_blob->y);
		float dist = distance(r->position2d->px, r->position2d->py, r->acquired_blob->x, r->acquired_blob->y);
		
		if (dist < 3) {
			r->state = GOING_TO_BLOB;
			set_speed(r, r->vlong / 2);
		}
	}
	else if (r->state == GOING_TO_BLOB) {
		// Find correct position and go for it
		// Very close, use own camera
		int k, temp_diff = 0;
		float temp_range = 0;
		for(k = 0; k < r->bf->blobs_count; k++){
			if (r->bf->blobs[k].color == 65280){
				temp_diff = 40 - (int)(r->bf->blobs[k].x);
				temp_range = r->bf->blobs[k].range;
			}
		}

		if (temp_diff < 0) turn_right(r);
		else if (temp_diff > 0) turn_left(r);
		else no_turn(r);
		r->vrot /= 2;

		// If is close enough and facing blob
		if (temp_range < 0.52f) {
			no_turn(r);
			set_speed(r, 0);
			r->state = GRABBING_BLOB;
		}
	}
	else if (r->state == GRABBING_BLOB) {
		// Close gripper if it's not closing already
		if (!r->isClosingGripper) {
			close_gripper(r);
		}
		if (is_gripper_closed(r)) {
			r->state = GOING_TO_BASE;
			is_robot_acting = 0;
		}
		
	}
	else if (r->state == GOING_TO_BASE) {
		// Go to (0,0)
		go_to (r, 0.f, 0.f);
		if (distance(r->position2d->px, r->position2d->py, 0.f, 0.f) < 1) {
			r->state = DROPPING_BLOB;
		}
	}
	else if (r->state == DROPPING_BLOB) {
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
		// Go to initial position
		go_to(r, r->initial_x, r->initial_y);
	}
}

void no_turn (ROBOT* r) {
	r->vrot = 0;
}

void turn_left (ROBOT* r) {
	r->vrot = 0.4f;
}

void turn_right (ROBOT* r) {
	r->vrot = -0.4f;
}

void set_speed (ROBOT* r, float val) {
	r->vlong = val;
}

int go_to (ROBOT* r, float x, float y) {
	r->dest_x = x;
	r->dest_y = y;
	
	r->vlong = r->max_speed;
	
	//Calcula vel longitudinal
	float dist = distance(r->position2d->px, r->position2d->py, x, y);
	//printf("[%d] Pos: %f,%f - Dest: %f,%f - Dist: %f\n", r->port, r->position2d->px, r->position2d->py, x, y, dist);
	
	if (dist < 0.8) {
		return 1;
	}

	//Calcula força de atração
	float angdest = atan2(y - r->position2d->py, x - r->position2d->px);
	float ang_rot = r->position2d->pa - angdest;

	printf("AngRot: %f\n", ang_rot);
	
	r->vrot = -ang_rot;
	//printf("AngDest: %f, AngRot: %f\n", angdest, ang_rot);

	//Calcula força de repulsão
	float campo_obst=0;

	int i;
	for(i=200; i<=360; i+=30) {
		if (r->laser->ranges[i] < 2.0)
			campo_obst += 2.0 - r->laser->ranges[i];
	}

	for(i=160; i>=0; i-=30) {
		if (r->laser->ranges[i] < 2.0)
			campo_obst -= 2.0 - r->laser->ranges[i];
	}

	r->vrot -= 0.5 * campo_obst;
	
	return 0;
}

float diff (float a, float b) {
	float res = a - b;
	
	if (res < 0) return -res;
	else return res;
}

float distance (float x1, float y1, float x2, float y2) {
	return sqrt(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
}

void execute (ROBOT* r) {
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
