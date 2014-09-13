TARGETS = t_setpriority sched_set sched_view
OBJS = t_setpriority.o sched_set.o sched_view.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_setpriority: t_setpriority.o
t_setpriority.o: t_setpriority.c
sched_set: sched_set.o
sched_set.o: sched_set.c
sched_view: sched_view.o
sched_view.o: sched_view.c
