#ifndef BLOB_H
#define BLOB_H

#include <stdlib.h>

typedef struct blob {
	float x, y;			// Position
	int is_acquired;	// If someone's going to get it
	
	int id;				// Useless - should be identifier
} BLOB;


BLOB* create_blob (float x, float y, int id);

void acquire (BLOB* b);

void delete_blob (BLOB* b);

#endif













