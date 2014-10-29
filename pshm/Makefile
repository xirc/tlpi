TARGETS = pshm_create pshm_write pshm_read pshm_unlink
OBJS = pshm_create.o pshm_write.o pshm_read.o pshm_unlink.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -lrt
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

pshm_create: pshm_create.o
pshm_create.o: pshm_create.c
pshm_write: pshm_write.o
pshm_write.o: pshm_write.c
pshm_read: pshm_read.o
pshm_read.o: pshm_read.c
pshm_unlink: pshm_unlink.o
pshm_unlink.o: pshm_unlink.c
