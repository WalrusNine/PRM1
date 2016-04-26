all: main	

main:	
	gcc -o main blob.c bloblist.c robot.c main.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

gripper:
	gcc -o gripper gripper.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

clean:
	rm *~ *.o main gripper
