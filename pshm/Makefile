TARGETS = pshm_create pshm_write
OBJS = pshm_create.o pshm_write.o
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
