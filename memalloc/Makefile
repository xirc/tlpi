TARGETS = free_and_sbrk free_and_sbrk2 malloc_and_free
OBJS = free_and_sbrk.o free_and_sbrk2.o malloc_and_free.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

free_and_sbrk: free_and_sbrk.o
free_and_sbrk.o: free_and_sbrk.c
free_and_sbrk2: free_and_sbrk2.o
free_and_sbrk2.o: free_and_sbrk2.c
malloc_and_free: malloc_and_free.o
malloc_and_free.o: malloc_and_free.c
