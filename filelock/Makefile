TARGETS = t_flock i_fcntl_locking
OBJS = t_flock.o i_fcntl_locking.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_flock: t_flock.o
t_flock.o: t_flock.c
i_fcntl_locking: i_fcntl_locking.o
i_fcntl_locking.o: i_fcntl_locking.c
