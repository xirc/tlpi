TARGETS = t_fork fork_file_sharing t_vfork fork_whos_on_first
OBJS = t_fork.o fork_file_sharing.o t_vfork.o fork_whos_on_first.o
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
