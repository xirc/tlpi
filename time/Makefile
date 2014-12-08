TARGETS = calendar_time
OBJS = calendar_time.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

calendar_time: calendar_time.o
calendar_time.o: calendar_time.c
