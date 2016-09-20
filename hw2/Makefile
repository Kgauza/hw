SRC=mm_alloc.c main.c
EXEC=hw2

CC=gcc
CFLAGS=-g -Wall
LDFLAGS=

OBJS=$(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@  

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXEC) $(OBJS)
