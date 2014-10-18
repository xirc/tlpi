TARGETS = mmcat t_mmap
OBJS = mmcat.o t_mmap.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

mmcat: mmcat.o
mmcat.o: mmcat.c
t_mmap: t_mmap.o
t_mmap.o: t_mmap.c
