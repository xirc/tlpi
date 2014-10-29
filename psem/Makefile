TARGETS = psem_create psem_unlink psem_wait psem_post psem_getvalue
OBJS = psem_create.o psem_unlink.o psem_wait.o psem_post.o psem_getvalue.o
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
psem_wait: psem_wait.o
psem_wait.o: psem_wait.c
psem_post: psem_post.o
psem_post.o: psem_post.c
psem_getvalue: psem_getvalue.o
psem_getvalue.o: psem_getvalue.c
