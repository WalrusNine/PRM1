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
		// Add as first, because list is empty
		l->first = n;
		return;
	}
	
	while(aux->next != NULL) {
		// Check if aux is n (if is already in list)
		if (aux->blob->id == n->blob->id) {
			// Delete node
			delete_node(n);
			// Don't add
			return;
		}
		else aux = aux->next;
	}
	// If reached here, blob isn't in list yet, so add it
	aux->next = n;
}

BLOB* get_unacquired_blob (BLOBLIST* l) {
	NODE* aux = l->first;
	
	while (aux != NULL) {
		// Run through list, searching for unacquired blob
		// So that a robot can reserve it
		if (!(aux->blob->is_acquired)) {
			return aux->blob;
		}
		aux = aux->next;
	}
	
	// If reached here, no unacquired blob was found
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
