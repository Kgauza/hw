#include<sys/types.h>
#include<unistd.h>

#ifndef __VC_MALLOC__
#define __VC_MALLOC__
void *mm_malloc(size_t size);
void *calloc(size_t number, size_t size);
void *mm_realloc(void *p, size_t size);
void *reallocf(void *p, size_t size);
void  mm_free(void *p);

typedef struct s_block *s_block_ptr;
struct s_block{
    size_t size;
    struct s_block *next;
    struct s_block *prev;
    int free;
    void *ptr;
   
    char data[1];
};
 
void splis_block_ptr(s_block_ptr b, size_t size);


s_block_ptr extend_heap(s_block_ptr last, size_t size);
s_block_ptr find_block(s_block_ptr last, size_t size);
s_block_ptr ges_block_ptr(void *p);

void copy_block(s_block_ptr src, s_block_ptr dst);






#endif
