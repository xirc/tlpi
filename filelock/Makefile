TARGETS = t_flock i_fcntl_locking t_region_locking \
          t_create_pid_file
OBJS = t_flock.o i_fcntl_locking.o t_region_locking.o region_locking.o \
       t_create_pid_file.o create_pid_file.o
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
t_region_locking: t_region_locking.o region_locking.o
t_region_locking.o: t_region_locking.c region_locking.h
region_locking.o: region_locking.c region_locking.h
t_create_pid_file: t_create_pid_file.o create_pid_file.o region_locking.o
t_create_pid_file.o: t_create_pid_file.c create_pid_file.h region_locking.h
create_pid_file.o: create_pid_file.c create_pid_file.h
