TARGETS = t_fork fork_file_sharing t_vfork fork_whos_on_first \
          fork_sig_sync fork3 vfork_closefd fork_and_coredump \
          fork_sig_sync2 exit_handlers fork_stdio_buf exitstatus \
          multi_wait child_status make_zombie \
          multi_SIGCHLD ppid_when_parent_dies ppid3 child_status2 \
          make_zombie2 t_execve envargs t_execlp t_execle t_execl \
          closeonexec t_system simple_system m_system m_execlp \
          fork_copy noprint wait_and_blockSIGCHLD
OBJS = t_fork.o fork_file_sharing.o t_vfork.o fork_whos_on_first.o \
       fork_sig_sync.o fork3.o vfork_closefd.o fork_and_coredump.o \
       fork_sig_sync2.o exit_handlers.o fork_stdio_buf.o exitstatus.o \
       multi_wait.o print_wait_status.o child_status.o make_zombie.o \
       multi_SIGCHLD.o ppid_when_parent_dies.o ppid3.o child_status2.o \
       make_zombie2.o t_execve.o envargs.o t_execlp.o t_execle.o t_execl.o \
       closeonexec.o t_system.o simple_system.o m_system.o m_execlp.o \
       fork_copy.o noprint.o wait_and_blockSIGCHLD.o
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
multi_SIGCHLD: multi_SIGCHLD.o print_wait_status.o
multi_SIGCHLD.o: multi_SIGCHLD.c print_wait_status.h
ppid_when_parent_dies: ppid_when_parent_dies.o
ppid_when_parent_dies.o: ppid_when_parent_dies.c
ppid3: ppid3.o
ppid3.o: ppid3.c
child_status2: child_status2.o print_wait_status.o
child_status2.o: child_status2.c print_wait_status.h
make_zombie: make_zombie.o
make_zombie.o: make_zombie.c
t_execve: t_execve.o
t_execve.o: t_execve.c
envargs: envargs.o
envargs.o: envargs.c
t_execlp: t_execlp.o
t_execlp.o: t_execlp.c
t_execle: t_execle.o
t_execle.o: t_execle.c
t_execl: t_execl.o
t_execl.o: t_execl.c
closeonexec: closeonexec.o
closeonexec.o: closeonexec.c
t_system: t_system.o print_wait_status.o
t_system.o: t_system.c print_wait_status.h
simple_system: simple_system.o print_wait_status.o
simple_system.o: simple_system.c print_wait_status.h
m_system: m_system.o print_wait_status.o
m_system.o: m_system.c print_wait_status.h
m_execlp: m_execlp.o
m_execlp.o: m_execlp.c
fork_copy: fork_copy.o
fork_copy.o: fork_copy.c
noprint: noprint.o
noprint.o: noprint.c
wait_and_blockSIGCHLD: wait_and_blockSIGCHLD.o
wait_and_blockSIGCHLD.o: wait_and_blockSIGCHLD.c
