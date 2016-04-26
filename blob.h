#ifndef BLOB_H
#define BLOB_H

#include <stdlib.h>

typedef struct blob {
	float x, y;
	int is_acquired;
	
	int id;
} BLOB;

BLOB* create_blob (float x, float y, int id);

void acquire (BLOB* b);

void delete_blob (BLOB* b);

#endif













