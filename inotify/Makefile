TARGETS = demo_inotify demo_inotify_r
OBJS = demo_inotify.o demo_inotify_r.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

demo_inotify: demo_inotify.o
demo_inotify.o: demo_inotify.c
demo_inotify_r: demo_inotify_r.o
demo_inotify_r.o: demo_inotify_r.c
