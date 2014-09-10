TARGETS = t_pgrp t_setsid t_ctermid catch_SIGHUP
OBJS = t_pgrp.o t_setsid.o t_ctermid.o catch_SIGHUP.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_pgrp: t_pgrp.o
t_pgrp.o: t_pgrp.c
t_setsid: t_setsid.o
t_setsid.o: t_setsid.c
t_ctermid: t_ctermid.o
t_ctermid.o: t_ctermid.c
catch_SIGHUP: catch_SIGHUP.o
catch_SIGHUP.o: catch_SIGHUP.c
