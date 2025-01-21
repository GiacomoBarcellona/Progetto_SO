
CC = gcc

# sorgenti
SRCS = bitmap.c buddy_allocator.c pseudo_malloc.c malloc_test.c

# oggetto 
OBJS = bitmap.o buddy_allocator.o pseudo_malloc.o malloc_test.o
 
all: malloc_test

malloc_test: $(OBJS)
	$(CC) -o $@ $^ -lm

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) malloc_test

.PHONY: all clean 