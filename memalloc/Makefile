TARGETS = free_and_sbrk
OBJS = free_and_sbrk.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

free_and_sbrk: free_and_sbrk.o
free_and_sbrk.o: free_and_sbrk.c
