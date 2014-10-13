TARGETS = svmsg_demo_server t_ftok
OBJS = svmsg_demo_server.o t_ftok.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

svmsg_demo_server: svmsg_demo_server.o
svmsg_demo_server.o: svmsg_demo_server.c
t_ftok: t_ftok.o
t_ftok.o: t_ftok.c
