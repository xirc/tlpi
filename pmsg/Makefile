TARGETS = pmsg_unlink pmsg_create pmsg_getattr pmsg_send
OBJS = pmsg_unlink.o pmsg_create.o pmsg_getattr.o pmsg_send.o
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
pmsg_create: pmsg_create.o
pmsg_create.o: pmsg_create.c
pmsg_getattr: pmsg_getattr.o
pmsg_getattr.o: pmsg_getattr.c
pmsg_send: pmsg_send.o
pmsg_send.o: pmsg_send.c
