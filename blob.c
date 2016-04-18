#include "blob.h"

BLOB* create_blob (float x, float y, int id) {
	BLOB* b = malloc(sizeof(BLOB));
	
	// Setup position
	b->x = x;
	b->y = y;
	b->id = id;
	
	b->is_acquired = 0;
	
	return b;
}

void acquire (BLOB* b) {
	b->is_acquired = 1;
}

void delete_blob (BLOB* b) {
	free(b);
}