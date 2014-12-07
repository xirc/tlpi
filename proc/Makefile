TARGETS = mem_segments necho display_env modify_env longjmp
OBJS = mem_segments.o necho.o display_env.o modify_env.o longjmp.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

mem_segments: mem_segments.o
mem_segments.o: mem_segments.c
necho: necho.o
necho.o: necho.c
display_env: display_env.o
display_env.o: display_env.c
modify_env: modify_env.o
modify_env.o: modify_env.c
longjmp: longjmp.o
longjmp.o: longjmp.c
