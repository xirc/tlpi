TARGETS = pmsg_unlink
OBJS = pmsg_unlink.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -lrt
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

pmsg_unlink: pmsg_unlink.o
pmsg_unlink.o: pmsg_unlink.c
