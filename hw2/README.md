# Homework 2: Memory Allocator #

**Due: 14 October 2016, 11:59 PM**

## 1 Introduction

General-purpose memory allocators can become bottlenecks in high-performance applications, so custom memory managers can be written to fit a particular application's memory usage.
For this assignment, you will implement a dynamic memory allocator.
This will provide insight into how heap memory management for an application is performed and reasoning behind allocator desgin.

## 2 Background

Every process has its own virtual address space. Parts of this space are mapped to physical memory locations through address translation, which is managed by the memory management unit and the kernel. 
You have seen first-hand in homework 0 that the address space is divided into several parts, primarily the stack, heap, and segments for code and static data.

<p align="center"><img src="./images/memory.png" alt="Memory Layout" align="middle"></p>

### 2.1 Heap

The heap is a continous region of the application's address space that is dynamically allocated during runtime by the program.  
It has three bounds:
- The start/'bottom' of the heap
- The 'top' of the heap, called the *break* or *brk*. It marks the end of the currently mapped memory region. This is a soft limit and can be increased using the brk() and sbrk() functions.
- A hard limit, an upper limit to the heap which the break cannot surpass. This can be managed through the getrlimit() and setrlimit() functions in &lt;sys/resource.h&gt;. You don't have to worry about it for this assignment.

<p align="center"><img src="./images/heap.png" alt="Heap"></p>

### 2.2 No Man's Land

The virutal address space is mapped to physical space by means of pages. When sbrk() is called to increase the heap limit, the new space is mapped to physical memory in quanta of pages. 
These pages, however, are of a fixed size (usually 4KB), meaning the heap break won't always be placed at a page boundary. 
A space can therefore arise between the break and the next page boundary: "no man's land".
While accessing addresses beyond the break should trigger memory access violation errors ("bus error" or "segmentaition fault"), no errors are raised for memory accesses that are in "no man's land", outside of the mapped heap region but within an allocated page. Be aware that this can cause bugs to occur that are very difficult to track.

<p align="center"><img src="./images/nomansland.png" alt="No Man's Land" align="middle"></p>

### 2.3 sbrk

Initially, the application's heap will not be allocated. The sbrk() function is what you will be using to increase the mapped region of the heap.
The syscall has the following prototype:  
```void *sbrk(int increment)```

The function manipulates the position of the break, specifically by incrementing its position by a specified number of **bytes**. 
It returns the address of the **previous** break; so you can use ```sbrk(0)``` to get the current position of the break.
Read the man page for more information.

## 3 Buddy Allocator

The buddy memory allocation algorithm works by dividing memory partitions of specific sizes in order to fit the requested memory size suitably. The Linux kernel uses a modified version of the buddy system.  

Consider a binary buddy system where each block is a power of two. You start off with a single block of some maximum block size, say 2<sup>n</sup>. 
The system works by subdividing this block to best fit the sizes of memory allocations to powers of 2.
A block's buddy is its immediate neighbour formed during a split. Buddy blocks are merged when both are free, this reduces external fragmentation.

Whenever a request for a memory allocation is performed:
- The size of the request is rounded up to the next power of two, say 2<sup>k</sup>
- Search for the most suitable block size of 2<sup>k</sup>
- If found, allocate the memory block
- If not found:
    - Split a free memory block larger than the requested memory size in half
    - Repeat until at the correct block size of 2<sup>k</sup>
    
Whenever memory is freed:
- Ensure the memory location is valid to be freed
- If the block's buddy is free:
    - Merge the two blocks
    - Attempt to merge the buddy of the merged block until the upper block limit is reached or the buddy is not free

The main advantage of buddy allocation is fast block merging, since 'buddy' blocks are coalesced without needing to traverse the entire list looking for free blocks.
The main disadvantage of buddy allocators is that they suffer from internal fragmentation, because allocations are rounded up to the nearest block size. 
For example, a 68-byte allocation will require a 128-byte block.

### Examples:

Consider a buddy system where the maximum block size is 256KiB. For simplicity we illustrate using a first-fit algorithm for allocation, that is, we choose the first block we encounter in the list that is larger or equal to the requested size.

- Say space for 32KiB is requested in our program. The size is already a power of 2, so it isn't rounded up. Since the 256KiB block is too big, it is divided into two 128KiB blocks on step 1. We continue subdividing blocks until at the suitable size of 32KiB. That block is then marked as allocated.

<p align="center"><img src="./images/buddysplit.png" alt="Buddy Split" align="middle"></p>

- 42KiB is requested. The size is rounded up to 64KiB. We scan across the list of blocks: the first 32KiB blocks are too small, then we find the 64KiB block fits, so it is allocated.

<p align="center"><img src="./images/buddyalloc.png" alt="Buddy Alloc" align="middle"></p>

We see internal fragmantation here. Since the size was rounded up, 22KiB of space is wasted in the block.

<p align="center"><img src="./images/internalfragmentation.png" alt="Internal Fragmentation" align="middle"></p>

- The first 32KiB block is freed. Its buddy is checked; since it is free, the two blocks are merged into a 64KiB block. The buddy of the merged block is not free, so we cannot merge again.

<p align="center"><img src="./images/buddyfree.png" alt="Buddy Free" align="middle"></p>

- The 64KiB block is freed. Its buddy is free, so they are merged. We continue merging until at the maximum block size since all the buddies are free.

<p align="center"><img src="./images/buddymerge.png" alt="Buddy Merge" align="middle"></p>

## 4 Tasks

You will use the buddy memory allocation technique for your custom allocator.  
There are numerous versions of this algorithm, we have provided skeleton code with a structure for a simple linked list of blocks.

### 4.1 Setup

Download the provided skeleton code:  
```
$ git clone https://github.com/WITS-COMS2001/hw.git
$ cd hw/hw2
```

We have provided a very simple program in main.c to test your allocator. It will produce various errors until you implement the functions in mm_alloc.c. You can run it as follows:  
```
$ make
$ ./hw2
```

### 4.2 Allocation

Implement buddy memory allocation as described above. We have defined a block structure for you to use in mm_alloc.h

```
void* mm_malloc(size_t size)
```
The function takes in the reqeusted memory size in number of bytes (where size_t represents an unsigned integer type) and returns a pointer to a valid memory location with at least that many bytes available.  

Return NULL if the requested size is zero or cannot be allocated.

- **Implement the mm_malloc() function using the buddy allocation algorithm**

NOTES:
- We have predefined the maximum block size, MAX_SIZE, to be 1 MiB. 
- Remember to add the sizeof your block struct to the requested size; you need to fit the block metadata into the same space.  
- On the first mm_malloc request, extend the heap with ```sbrk()``` and create the maximum block. Afterwards, you can follow the normal sequence of allocating a block.  
- Use a first-fit algorithm when searching for a block: Search for the first block greater or equal in size to allocate, and split the block as usual if greater.  
- When initialising a block, initialise the 'merged_buddy' array elements to NULL.
- When splitting a block, add the buddy of the block being split to the 'merged_buddy' arrays of the split blocks. This aids in the merging process.
- Beware of pointer arithmetic behaviour!

### 4.3 Deallocation

```
void mm_free( void* ptr )
```  
The function takes in a pointer to a memory block previously allocated using mm_alloc() and frees it, making it available for allocation again.
Ensure that only pointers to valid blocks are freed. If ptr is NULL, do nothing.
To reduce fragmentation and use memory efficiently, blocks should be merged when when freed.  

- **Implement the mm_free() function**
- **Implement block merging on free**

NOTES:
- When a block is freed, check its buddy. If the buddy is also free, merge the blocks together.  
- If the buddy is not the same size as the current block, obviously it cannot be merged since it is split.
- Check on the larger merged blocks until the buddy is not free, or at the largest block size.  
- You should use the 'merged_buddy' array to hold the buddies of the merged blocks.
- Keep the 'merged_buddy' array updated as blocks are split and merged.

### 4.4 Reallocation

```
void* mm_realloc(void* ptr, size_t size)
```
The function takes in a pointer to a memory block previously allocated using mm_alloc() and returns a pointer to the data in a block of at least size size.  

- **Implement the mm_realloc() function**

NOTES:
- Remember to round up the size
- The contents of the old block must be copied into the new block, hint: use ```memcpy```. You may use the same block if it fits the size. 
- The previous memory location must be deallocated if a new block is chosen.  
- mm_realloc() on a NULL pointer should behave the same as mm_alloc().

## 5 Submission

- Remove any extraneous output for debugging from your code. Calling any of your functions mm_alloc, mm_free, or mm_realloc should not print to stdout.
- Place all your source code into a folder called 'hw2' (case sensitive) in the root of your repository.
- Commit your changes and push to your private repository:  
```
git add .
git commit
git push https://github.com/WITS-COMS2001/<student_number> master
```
- Ensure that you include all files for your submission to be compiled. That is, if we run ```make``` on only the files you submitted, it should compile without errors. If your code does not compile or run, you will get 0.  

This assignment is due **14 October 2016, 11:59 PM**. Please ensure your final code is in your private repository by then! 