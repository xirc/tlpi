TARGETS = copy seek_io tee cp \
          bad_exclusive_open t_readv large_file \
          large_file2 append_seek_write atomic_append t_mdup \
          multi_descriptors m_rdwrv
OBJS = copy.o seek_io.o tee.o cp.o \
       bad_exclusive_open.o t_readv.o large_file.o \
       large_file2.o append_seek_write.o atomic_append.o \
       t_mdup.o mdup.o multi_descriptors.o m_rdwrv.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

copy: copy.o
copy.o: copy.c
seek_io: seek_io.o
seek_io.o: seek_io.c
tee: tee.o
tee.o: tee.c
cp: cp.o
cp.o: cp.c
bad_exclusive_open: bad_exclusive_open.o
bad_exclusive_open.o: bad_exclusive_open.c
t_readv: t_readv.o
t_readv.o: t_readv.c
large_file: large_file.o
large_file.o: large_file.c
large_file2: large_file2.o
large_file2.o: large_file2.c
	$(CC) $(CFLAGS) -D_FILE_OFFSET_BITS=64 $(LDFLAGS) -c -o $@ $^
append_seek_write: append_seek_write.o
append_seek_write.o: append_seek_write.c
atomic_append: atomic_append.o
atomic_append.o: atomic_append.c
t_mdup: t_mdup.o mdup.o
t_mdup.o: t_mdup.c
mdup.o: mdup.c mdup.h
multi_descriptors: multi_descriptors.o
multi_descriptors.o: multi_descriptors.c
m_rdwrv: m_rdwrv.o
m_rdwrv.o: m_rdwrv.c
