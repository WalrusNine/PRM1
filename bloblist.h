#ifndef BLOBLIST_H
#define BLOBLIST_H

#include "blob.h"

// Dynamic list's element
typedef struct node {
	// Blob
	struct blob* blob;
	// Next element
	struct node* next;
} NODE;

// Dynamic list
typedef struct bloblist {
	struct node* first;
	int size;
} BLOBLIST;

BLOBLIST* create_list ();

NODE* create_node (BLOB* b);

void add_node (BLOBLIST* l, NODE* n);

// Robot reserves a blob
BLOB* get_unacquired_blob (BLOBLIST* l);



void delete_list (BLOBLIST* l);

void delete_node (NODE* n);

#endif













