TARGETS = ouch intquit t_kill t_signal_functions \
          sig_sender sig_receiver sig_receiver2 sig_receiver_ign \
          sig_receiver_nodefer sigreset m_siginterrupt \
          nonreentrant sigmask_longjmp t_sigaltstack m_abort \
          t_sigqueue catch_rtsigs
OBJS = ouch.o intquit.o t_kill.o t_signal_functions.o signal_functions.o \
       sig_sender.o sig_receiver.o sig_receiver2.o sig_receiver_ign.o \
       sig_receiver_nodefer.o sigreset.o m_siginterrupt.o \
       nonreentrant.o sigmask_longjmp.o t_sigaltstack.o m_abort.o \
       t_sigqueue.o catch_rtsigs.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

ouch: ouch.o
ouch.o: ouch.c
intquit: intquit.o
intquit.o: intquit.c
t_kill: t_kill.o
t_kill.o: t_kill.c
t_signal_functions: t_signal_functions.o signal_functions.o
t_signal_functions.o: t_signal_functions.c signal_functions.h
signal_functions.o: signal_functions.c signal_functions.h
sig_sender: sig_sender.o
sig_sender.o: sig_sender.c
sig_receiver: sig_receiver.o signal_functions.o
sig_receiver.o: sig_receiver.c signal_functions.h
sig_receiver2: sig_receiver2.o signal_functions.o
sig_receiver2.o: sig_receiver2.c signal_functions.h
sig_receiver_ign: sig_receiver_ign.o signal_functions.o
sig_receiver_ign.o: sig_receiver_ign.c signal_functions.h
sig_receiver_nodefer: sig_receiver_nodefer.o signal_functions.o
sig_receiver_nodefer.o: sig_receiver_nodefer.c signal_functions.h
sigreset: sigreset.o
sigreset.o: sigreset.c
m_siginterrupt: m_siginterrupt.o
m_siginterrupt.o: m_siginterrupt.c
nonreentrant: nonreentrant.o
	$(CC) -lcrypt -o $@ $^
nonreentrant.o: nonreentrant.c
sigmask_longjmp: sigmask_longjmp.o signal_functions.o
sigmask_longjmp.o: sigmask_longjmp.c signal_functions.h
t_sigaltstack: t_sigaltstack.o
t_sigaltstack.o: t_sigaltstack.c
m_abort: m_abort.o
m_abort.o: m_abort.c
t_sigqueue: t_sigqueue.o
t_sigqueue.o: t_sigqueue.c
catch_rtsigs: catch_rtsigs.o
catch_rtsigs.o: catch_rtsigs.c
