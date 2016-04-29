#include "bloblist.h"

#include <stdio.h>

BLOBLIST* create_list () {
	BLOBLIST* l = malloc(sizeof(BLOBLIST));
	
	l->first = NULL;
	l->size = 0;
	
	return l;
}

NODE* create_node (BLOB* b) {
	NODE* n = malloc(sizeof(NODE));
	
	n->blob = b;
	n->next = NULL;
}

void add_node (BLOBLIST* l, NODE* n) {
	// Find last
	NODE* aux = l->first;
	if (aux == NULL) {
		// Add as first
		l->first = n;
		return;
	}
	
	while(aux->next != NULL) {
		// Check if aux is n
		if (aux->blob->id == n->blob->id) {
			// Delete node
			delete_node(n);
			// Don't add
			return;
		}
		else aux = aux->next;
	}
	aux->next = n;
	//printf("%f,%f\n", n->blob->x, n->blob->y);
}

BLOB* get_unacquired_blob (BLOBLIST* l) {
	NODE* aux = l->first;
	
	while (aux != NULL) {
		if (!(aux->blob->is_acquired)) {
			return aux->blob;
		}
		aux = aux->next;
	}
	
	return NULL;
}

void delete_list (BLOBLIST* l) {
	NODE* aux = l->first;
	NODE* prem = NULL;
	
	while(aux != NULL) {
		prem = aux;
		aux = aux->next;
		delete_node(prem);
	}
}

void delete_node (NODE* n) {
	delete_blob(n->blob);
	free(n);
}
