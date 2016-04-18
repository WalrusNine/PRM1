#include "blob.h"

BLOB* create_blob (float x, float y) {
	BLOB* b = malloc(sizeof(BLOB));
	
	// Setup position
	b->world_x = x;
	b->world_y = y;
	
	b->self_x = 0;
	b->self_y = 0;
	
	b->is_acquired = 0;
	
	return b;
}

void acquire (BLOB* b) {
	b->is_acquired = 1;
}

void delete_blob (BLOB* b) {
	free(b);
}