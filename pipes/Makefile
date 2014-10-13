TARGETS = simple_pipe pipe_sync pipe_ls_wc popen_glob \
          fifo_seqnum_server fifo_seqnum_client echo_upcase \
          m_popen_pclose fifo_seqnum_server2 fifo_seqnum_client2
OBJS = simple_pipe.o pipe_sync.o pipe_ls_wc.o popen_glob.o \
       fifo_seqnum_server.o fifo_seqnum_client.o echo_upcase.o \
       m_popen_pclose.o fifo_seqnum_server2.o fifo_seqnum_client2.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

simple_pipe: simple_pipe.o
simple_pipe.o: simple_pipe.c
pipe_sync: pipe_sync.o
pipe_sync.o: pipe_sync.c
pipe_ls_wc: pipe_ls_wc.o
pipe_ls_wc.o: pipe_ls_wc.c
popen_glob: popen_glob.o
popen_glob.o: popen_glob.c
fifo_seqnum_server: fifo_seqnum_server.o
fifo_seqnum_server.o: fifo_seqnum_server.c fifo_seqnum.h
fifo_seqnum_client: fifo_seqnum_client.o
fifo_seqnum_client.o: fifo_seqnum_client.c fifo_seqnum.h
echo_upcase: echo_upcase.o
echo_upcase.o: echo_upcase.c
m_popen_pclose: m_popen_pclose.o
m_popen_pclose.o: m_popen_pclose.c
fifo_seqnum_server2: fifo_seqnum_server2.o
fifo_seqnum_server2.o: fifo_seqnum_server2.c fifo_seqnum.h
fifo_seqnum_client2: fifo_seqnum_client2.o
fifo_seqnum_client2.o: fifo_seqnum_client2.c fifo_seqnum.h
