#ifndef BLOB_H
#define BLOB_H

typedef struct blob {
	float world_x, world_y;
	float self_x, self_y;
	int is_acquired;
} BLOB;

BLOB* create_blob (float x, float y);

void acquire (BLOB* b);

void delete_blob (BLOB* b);

#endif













