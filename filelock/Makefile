TARGETS = t_flock i_fcntl_locking t_region_locking \
          t_create_pid_file flock_deadlock flock_dup flock_open2 \
          fcntl_locking_spec_a fcntl_locking_spec_b
OBJS = t_flock.o i_fcntl_locking.o t_region_locking.o region_locking.o \
       t_create_pid_file.o create_pid_file.o flock_deadlock.o flock_dup.o flock_open2.o \
          fcntl_locking_spec_a.o fcntl_locking_spec_b.o
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
flock_deadlock: flock_deadlock.o
flock_deadlock.o: flock_deadlock.c
flock_dup: flock_dup.o
flock_dup.o: flock_dup.c
flock_open2: flock_open2.o
flock_open2.o: flock_open2.c
fcntl_locking_spec_a: fcntl_locking_spec_a.o
fcntl_locking_spec_a.o: fcntl_locking_spec_a.c
fcntl_locking_spec_b: fcntl_locking_spec_b.o
fcntl_locking_spec_b.o: fcntl_locking_spec_b.c
