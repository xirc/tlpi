TARGETS = calendar_time t_curr_time
OBJS = calendar_time.o t_curr_time.o curr_time.o
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
t_curr_time: t_curr_time.o curr_time.o
t_curr_time.o: t_curr_time.c
curr_time.o: curr_time.c
