TARGETS = t_ugid_functions check_password
OBJS = ugid_functions.o t_ugid_functions.o check_password.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

t_ugid_functions: t_ugid_functions.o ugid_functions.o
t_ugid_functions.o: t_ugid_functions.c ugid_functions.h
ugid_functions.o: ugid_functions.c ugid_functions.h
check_password: check_password.o
	$(CC) -lcrypt -o $@ $^
check_password.o: check_password.c
