TARGETS = ouch intquit t_kill t_signal_functions \
          sig_sender sig_receiver
OBJS = ouch.o intquit.o t_kill.o t_signal_functions.o signal_functions.o \
       sig_sender.o sig_receiver.o
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
