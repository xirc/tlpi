TARGETS = copy seek_io tee
OBJS = copy.o seek_io.o tee.o
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
