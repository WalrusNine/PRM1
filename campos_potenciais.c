#include <stdio.h>
#include <libplayerc/playerc.h>
#include <math.h>

#include "bloblist.h"
#include "blob.h"
#include "robot.h"

/*
	Read, Update and Execute
	blob add acquired variable
	create hotspots for searching
*/

int main(int argc, const char **argv)
{
  int i, k;

  float vlong=0, vrot=0;
  float dest_x, dest_y, distobst, angdest, campo_obst, ang_rot;

	
    playerc_client_read(client);
    playerc_client_read(client);

	unsigned int blob_x;
	float blob_range;
	int diff = 40;

	BLOB_LIST* blist = create_list();
	

	for(k = 0; k < bf->blobs_count; k++){
		if (bf->blobs[k].color == 65280){
			// Found blob
			blob_x = bf->blobs[k].x;
			blob_range = bf->blobs[k].range;

			// Get angle to blob
			diff = 40 - blob_x;
			float angle = diff;
			printf("Pre angle = %f\n", angle);
			angle *= (3.f / 4.f);
			angle = angle * (M_PI / 180.f); // Radians

			dest_x = position2d->px + blob_range * cos(angle);
			dest_y = position2d->py + blob_range * sin(angle);
			printf("[%d] Angle: %f, destx: %f, desty: %f, diff: %d, range: %f\n", k, angle, dest_x, dest_y, diff, blob_range);

			add_blob(blist, create_blob(dest_x, dest_y));
		}
	}

	int c = 0;

  while (1) {

    playerc_client_read(client);

	// If didn't find a blob, look for it
	// Find Blob
	
	// Adjust with camera
	for(k = 0; k < bf->blobs_count; k++){
		if (bf->blobs[k].color == 65280){
			diff = 40 - (int)(bf->blobs[k].x);
			printf("K: %d, Diff: %d, X: %d\n", k, diff, (int)(bf->blobs[k].x));
			blob_range = bf->blobs[k].range;
		}
	}

	vlong = 0.4f;
	//if (!c) {
		if (diff < 0) {
			// Turn right
			vrot = -0.4f;
		}
		else if (diff > 0) {
			// turn left
			vrot = 0.4f;
		}
		else {
			// Keep
			vrot = 0;
			c = 1;
		}
	//}

/*//Calcula vel longitudinal
    distobst=sqrt((position2d->px-dest_x)*(position2d->px-dest_x)+ (position2d->py-dest_y)*(position2d->py-dest_y)) ;

    if (distobst > 0.3) vlong=0.1;
    else {
		break;
	}
//Calcula força de atração
    angdest=atan2(dest_y-position2d->py, dest_x-position2d->px);
    ang_rot=position2d->pa-angdest;
    vrot = -ang_rot*0.6f;



//Calcula força de repulsão
    campo_obst=0;

    for(i=200; i<=360; i+=30) 
      if (laser->ranges[i] < 2.0)
	campo_obst += 2.0-laser->ranges[i];

    for(i=160; i>=0; i-=30) 
      if (laser->ranges[i] < 2.0)
	campo_obst -= 2.0-laser->ranges[i];

    vrot -= 0.5 * campo_obst;*/
	
	//printf("Diff: %d, X: %d, Range: %f\n", diff, bf->blobs[1].x, bf->blobs[1].range);
    //printf("\nDist:%.3f, Ang:%.3f, Obst:%.3f, Pos:%.3f, %.3f", distobst, ang_rot, campo_obst, position2d->px, position2d->py);



	if (blob_range < 0.4f) {
		vrot = 0;
		vlong = 0;
		playerc_position2d_set_cmd_vel(position2d, vlong, 0, vrot, 1);
		break;
	}



    playerc_position2d_set_cmd_vel(position2d, vlong, 0, vrot, 1);
    }	// While
	
	playerc_gripper_close_cmd(gripper);

	// Go back
	dest_x = 0;
	dest_y = 0;
	while(1) {
		playerc_client_read(client);

		//Calcula vel longitudinal
		distobst=sqrt((position2d->px-dest_x)*(position2d->px-dest_x)+ (position2d->py-dest_y)*(position2d->py-dest_y)) ;

		printf("Dist: %f, px: %f, py: %f\n", distobst, position2d->px, position2d->py);

		if (distobst > 1) vlong=0.4f;
		else {
			playerc_gripper_open_cmd(gripper);
			break;
		}

		//Calcula força de atração
		angdest=atan2(dest_y-position2d->py, dest_x-position2d->px);
		ang_rot=position2d->pa-angdest;
		vrot = -ang_rot*0.6f;



		//Calcula força de repulsão
		campo_obst=0;

		for(i=200; i<=360; i+=30) 
		  if (laser->ranges[i] < 2.0)
		campo_obst += 2.0-laser->ranges[i];

		for(i=160; i>=0; i-=30) 
		  if (laser->ranges[i] < 2.0)
		campo_obst -= 2.0-laser->ranges[i];

		vrot -= 0.5 * campo_obst;

		playerc_position2d_set_cmd_vel(position2d, vlong, 0, vrot, 1);
	}

	while (1) {
		playerc_client_read(client);	
	}
	
	return 0;
}





/*
// Find Blob
	int k;
	for(k = 0; k < bf->blobs_count; k++){
		if (bf->blobs[k].color == 65280){
			// Found blob
			blob_x = bf->blobs[k].x;
			blob_range = bf->blobs[k].range;

			// Get angle to blob
			diff = 40 - blob_x;
			float angle = diff;
			printf("Pre angle = %f\n", angle);
			angle = angle * (M_PI / 180); // Radians

			destx = position2d->px + blob_range * cos(angle);
			desty = position2d->py + blob_range * sin(angle);
			printf("Angle: %f, destx: %f, desty: %f, diff: %d\n", angle, destx, desty, diff);

			break;
		}
	}

*/















