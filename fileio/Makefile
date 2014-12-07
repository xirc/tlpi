TARGETS = copy seek_io tee cp \
          bad_exclusive_open t_readv
OBJS = copy.o seek_io.o tee.o cp.o \
       bad_exclusive_open.o t_readv.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)

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
