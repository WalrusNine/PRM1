#include <stdio.h>
#include <math.h>

#include "bloblist.h"
#include "blob.h"
#include "robot.h"

/*
	create hotspots for searching
*/

int main(int argc, const char **argv) {
	// Create robots
	ROBOT* robot1 = create_robot(6665, WITH_GRIPPER);
	//ROBOT* robot2 = create_robot(6666, WITH_GRIPPER);
	ROBOT* cotoco = create_robot(6667, WITHOUT_GRIPPER);
	
	// Create bloblist
	BLOBLIST* blist = create_list();
	
	// Main loop
	while(1) {
		// Read
		robot_read(robot1);
		//robot_read(robot2);
		robot_read(cotoco);
		
		// Think
		update(robot1, blist);
		//update(robot2, blist);
		update(cotoco, blist);
		
		// Do
		execute(robot1);
		//execute(robot2);
		execute(cotoco);
	}
	
	delete_robot(robot1);
	//delete_robot(robot2);
	delete_robot(cotoco);
	
	delete_list(blist);
	
	return 0;
}
