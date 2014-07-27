TARGETS = t_fork fork_file_sharing t_vfork fork_whos_on_first \
          fork_sig_sync fork3 vfork_closefd fork_and_coredump \
          fork_sig_sync2 exit_handlers fork_stdio_buf exitstatus \
          multi_wait child_status make_zombie
OBJS = t_fork.o fork_file_sharing.o t_vfork.o fork_whos_on_first.o \
       fork_sig_sync.o fork3.o vfork_closefd.o fork_and_coredump.o \
       fork_sig_sync2.o exit_handlers.o fork_stdio_buf.o exitstatus.o \
       multi_wait.o print_wait_status.o child_status.o make_zombie.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

t_fork: t_fork.o
t_fork.o: t_fork.c
fork_file_sharing: fork_file_sharing.o
fork_file_sharing.o: fork_file_sharing.c
t_vfork: t_vfork.o
t_vfork.o: t_vfork.c
fork_whos_on_first: fork_whos_on_first.o
fork_whos_on_first.o: fork_whos_on_first.c
fork_sig_sync: fork_sig_sync.o
fork_sig_sync.o: fork_sig_sync.c
fork3: fork3.o
fork3.o: fork3.c
vfork_closefd: vfork_closefd.o
vfork_closefd.o: vfork_closefd.c
fork_and_coredump: fork_and_coredump.o
fork_and_coredump.o: fork_and_coredump.c
fork_sig_sync2: fork_sig_sync2.o
fork_sig_sync2.o: fork_sig_sync2.c
exit_handlers: exit_handlers.o
exit_handlers.o: exit_handlers.c
fork_stdio_buf: fork_stdio_buf.o
fork_stdio_buf.o: fork_stdio_buf.c
exitstatus: exitstatus.o
exitstatus.o: exitstatus.c
multi_wait: multi_wait.o
multi_wait.o: multi_wait.c
print_wait_status.o: print_wait_status.c print_wait_status.h
child_status: child_status.o print_wait_status.o
child_status.o: child_status.c print_wait_status.h
make_zombie: make_zombie.o
make_zombie.o: make_zombie.c
