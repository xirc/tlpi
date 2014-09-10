TARGETS = t_pgrp
OBJS = t_pgrp.o
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
