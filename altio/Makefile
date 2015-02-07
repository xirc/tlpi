TARGETS = t_select poll_pipes demo_sigio epoll_input \
          epoll_dupfd self_pipe select_pipes echo_server select_sysvmsgq \
         self_pipe2
OBJS = t_select.o poll_pipes.o demo_sigio.o tty_functions.o epoll_input.o \
       epoll_dupfd.o self_pipe.o select_pipes.o echo_server.o select_sysvmsgq.o \
       self_pipe2.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_select: t_select.o
t_select.o: t_select.c
poll_pipes: poll_pipes.o
poll_pipes.o: poll_pipes.c
tty_functions.o: tty_functions.c tty_functions.h
demo_sigio: demo_sigio.o tty_functions.o
demo_sigio.o: demo_sigio.c tty_functions.h
epoll_input: epoll_input.o
epoll_input.o: epoll_input.c
epoll_dupfd: epoll_dupfd.o
epoll_dupfd.o: epoll_dupfd.c
self_pipe: self_pipe.o
self_pipe.o: self_pipe.c
select_pipes: select_pipes.o
select_pipes.o: select_pipes.c
echo_server: echo_server.o
echo_server.o: echo_server.c
select_sysvmsgq: select_sysvmsgq.o
select_sysvmsgq.o: select_sysvmsgq.c
self_pipe2: self_pipe2.o
self_pipe2.o: self_pipe2.c
