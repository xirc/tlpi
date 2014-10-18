TARGETS = mmcat t_mmap anon_mmap t_mremap
OBJS = mmcat.o t_mmap.o anon_mmap.o t_mremap.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

mmcat: mmcat.o
mmcat.o: mmcat.c
t_mmap: t_mmap.o
t_mmap.o: t_mmap.c
anon_mmap: anon_mmap.o
anon_mmap.o: anon_mmap.c
t_mremap: t_mremap.o
t_mremap.o: t_mremap.c
