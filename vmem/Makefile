TARGETS = t_mprotect memlock t_memlockall t_madv_dontneed
OBJS = t_mprotect.o memlock.o t_memlockall.o t_madv_dontneed.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_mprotect: t_mprotect.o
t_mprotect.o: t_mprotect.c
memlock: memlock.o
memlock.o: memlock.c
t_memlockall: t_memlockall.o
t_memlockall.o: t_memlockall.c
t_madv_dontneed: t_madv_dontneed.o
t_madv_dontneed.o: t_madv_dontneed.c
