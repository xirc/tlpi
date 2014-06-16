TARGETS = mem_segments necho display_env modify_env longjmp \
          setjmp_vars setjmp_varsO bad_jmp
OBJS = mem_segments.o necho.o display_env.o modify_env.o longjmp.o \
       setjmp_vars.o setjmp_varsO.o bad_jmp.o
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
setjmp_vars: setjmp_vars.o
setjmp_vars.o: setjmp_vars.c
setjmp_varsO: setjmp_varsO.o
setjmp_varsO.o: setjmp_vars.c
	$(CC) $(CFLAGS) -O $(LDFLAGS) -c -o $@ $<
bad_jmp: bad_jmp.o
bad_jmp.o: bad_jmp.c
