TARGETS = t_ugid_functions check_password t_getpwuid m_getpwnam
OBJS = ugid_functions.o t_ugid_functions.o check_password.o \
       t_getpwuid.o m_getpwnam.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_ugid_functions: t_ugid_functions.o ugid_functions.o
t_ugid_functions.o: t_ugid_functions.c ugid_functions.h
ugid_functions.o: ugid_functions.c ugid_functions.h
check_password: check_password.o
	$(CC) -lcrypt -o $@ $^
check_password.o: check_password.c
t_getpwuid: t_getpwuid.o
t_getpwuid.o: t_getpwuid.c
m_getpwnam: m_getpwnam.o
m_getpwnam.o: m_getpwnam.c
