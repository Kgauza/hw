/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"
#define align4(x) ((((x) - 1) >> 2) << 2) + 4
#define BLOCK_SIZE 20
/* Your final implementation should comment out this macro. */
void *base = NULL;


s_block_ptr find_block(s_block_ptr last, size_t size){
    s_block_ptr b = base;
    while (b && !(b->free && b->size >= size)){
        last = b;
        b = b->next;
    }
    return b;
}

s_block_ptr extend_heap(s_block_ptr last, size_t size){
    size_t sb;
    s_block_ptr b;
    b = sbrk(0);
    sb = (size_t)sbrk(BLOCK_SIZE + size);
    if(sb < 0) return NULL;
    
    b->size = size;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;
    if(last) last->next = b;
    b->free = 0;
    return b;
}

void splis_block_ptr(s_block_ptr b, size_t size){
    s_block_ptr new;
    new = (s_block_ptr)(b->data + size);
    new->size = b->size - size - BLOCK_SIZE;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->ptr = new->data;
    b->size = size;
    b->next = new;
    if(new->next) new->next->prev = new;
}

void *mm_malloc(size_t size){
    s_block_ptr b, last;
    size_t s;
    s = align4(size);
    if(base){
        last = base;
        b = find_block(last, s);
        if(b){
            if((b->size - s) >= (BLOCK_SIZE + 4)) splis_block_ptr(b, s);
            b->free = 0;
        }
        else{
            b = extend_heap(last, s);
            if(!b) return NULL;
        }
    }
    else{
        b = extend_heap(NULL, s);
        if(!b) return NULL;
        base = b;
    }
    return b->data;
}


void *calloc(size_t number, size_t size){
    size_t *new;
    size_t s4, i;
    new = mm_malloc(number * size);
    if (new){
        s4 = align4(number * size) << 2;
        for (i = 0; i < s4; i++) new[i] = 0;
    }
    return new;
}

s_block_ptr fusion(s_block_ptr b){
    if(b->next && b->next->free){
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next) b->next->prev = b;
    }
    return b;
}

s_block_ptr ges_block_ptr(void *p){
    char *tmp;
    tmp = p;
    return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void *p){
    if (base){
        if(p > base && p < sbrk(0)) return (p == (ges_block_ptr(p))->ptr);
    }
    return 0;
}

void mm_free(void *p){
    s_block_ptr b;
    if (valid_addr(p)){
        b = ges_block_ptr(p);
        b->free = 1;
        if(b->prev && b->prev->free) b = fusion(b->prev);
        if(b->next){
            fusion(b);
        }
        else{
            if(b->prev) b->prev->next = NULL;
            else base = NULL;
            brk(b);
        }
    }
}

void copy_block(s_block_ptr src, s_block_ptr dst){
    int *sdata, *ddata;
    size_t i;
    sdata = src->ptr;
    ddata = dst->ptr;
    for(i = 0; i * 4 < src->size && i * 4 < dst->size; i++)
        ddata[i] = sdata[i];
}


void *mm_realloc(void *p, size_t size){
    size_t s;
    s_block_ptr b, new;
    void *newp;
    if(!p) return (mm_malloc(size));
    if (valid_addr(p)){
        s = align4(size);
        b = ges_block_ptr(p);
        if (b->size >= s){
            if(b->size - s >= (BLOCK_SIZE + 4)) splis_block_ptr(b, s);
        }
        else{
            if(b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size) >= s){
                fusion(b);
                if(b->size - s >= (BLOCK_SIZE + 4)) splis_block_ptr(b, s);
            }
            else{
                newp = mm_malloc(s);
                if (!newp) return NULL;
                new = ges_block_ptr(newp);
                copy_block(b, new);
                free(p);
                return newp;
            }
        }
        return p;
    }
    return NULL;
}


void *reallocf(void *p, size_t size){
    void *newp;
    newp = mm_realloc(p, size);
    if (!newp) free(p);
    return newp;
}
