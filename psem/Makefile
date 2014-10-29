TARGETS = psem_create psem_unlink
OBJS = psem_create.o psem_unlink.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -lrt
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

psem_create: psem_create.o
psem_create.o: psem_create.c
psem_unlink: psem_unlink.o
psem_unlink.o: psem_unlink.c
