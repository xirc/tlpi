TARGETS = t_fork fork_file_sharing t_vfork fork_whos_on_first \
          fork_sig_sync fork3 vfork_closefd
OBJS = t_fork.o fork_file_sharing.o t_vfork.o fork_whos_on_first.o \
       fork_sig_sync.o fork3.o vfork_closefd.o
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
