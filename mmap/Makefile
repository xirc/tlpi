TARGETS = mmcat t_mmap anon_mmap t_mremap mcp \
          mmap_xfr_reader mmap_xfr_writer t_mmap2
OBJS = mmcat.o t_mmap.o anon_mmap.o t_mremap.o mcp.o \
       mmap_xfr_reader.o mmap_xfr_writer.o binary_sems.o t_mmap2.o
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
mcp: mcp.o
mcp.o: mcp.c
mmap_xfr_reader: mmap_xfr_reader.o binary_sems.o
mmap_xfr_reader.o: mmap_xfr_reader.c mmap_xfr.h
mmap_xfr_writer: mmap_xfr_writer.o binary_sems.o
mmap_xfr_writer.o: mmap_xfr_writer.c binary_sems.o
binary_sems.o: binary_sems.c binary_sems.h
t_mmap2: t_mmap2.o
t_mmap2.o: t_mmap2.c
