TARGETS = simple_pipe pipe_sync
OBJS = simple_pipe.o pipe_sync.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

simple_pipe: simple_pipe.o
simple_pipe.o: simple_pipe.c
pipe_sync: pipe_sync.o
pipe_sync.o: pipe_sync.c
