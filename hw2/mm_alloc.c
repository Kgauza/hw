// mm_alloc.c
#include "mm_alloc.h"
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

/** mm_malloc - allocate a block of at least size bytes **/
void*
mm_malloc(size_t size) {
    // TODO
    return NULL;
}

/** mm_free - marks a data block as free **/
void
mm_free(void* ptr) {
    // TODO
}

/** mm_realloc - changes the data block to have the specified size **/
void*
mm_realloc(void* ptr, size_t size) {
    // TODO
    return NULL;
}

/*------------------------------------*\
|            DEBUG FUNCTIONS           |
\*------------------------------------*/

/** print_block - prints the metadata of a block **/
void
print_block(struct block* b) {
  if(!b) {
    printf("NULL block\n");
  }
  else {
    int i = 0;
    printf("Strt = %lx\n",(unsigned long) b);
    printf("Size = %lu\n",b->size);
    printf("Free = %s\n",(b->free)?"true":"false");
    printf("Data = %lx\n",(unsigned long) b->data);
    printf("Next = %lx\n",(unsigned long) b->next);
    printf("Buddy = %lx\n",(unsigned long) b->buddy);    
    printf("Merged Buddies = "); 
    while(b->merged_buddy[i] && i < MAX_EXP) {
        printf("%lx, ",(unsigned long) b->merged_buddy[i]);
        i++;
    }
    printf("\n\n");
  }
}

/** print_list - prints the metadata of all blocks **/
void
print_list() {
    struct block* curr = head;
    printf("--HEAP--\n");
    if(!head) printf("EMPTY\n");
    while(curr) {
        print_block(curr);
        curr = curr->next;
    }
    printf("--END--\n");
}
