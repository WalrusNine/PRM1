#ifndef BLOBLIST_H
#define BLOBLIST_H

#include "blob.h"

typedef struct node {
	struct blob* blob;
	struct node* next;
} NODE;

typedef struct bloblist {
	struct node* first;
	int size;
} BLOBLIST;

BLOBLIST* create_list ();

NODE* create_node (BLOB* b);

void add_node (BLOBLIST* l, NODE* n);


BLOB* get_unacquired_blob (BLOBLIST* l);



void delete_list (BLOBLIST* l);

void delete_node (NODE* n);

#endif













