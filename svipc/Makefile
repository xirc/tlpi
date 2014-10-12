TARGETS = svmsg_demo_server
OBJS = svmsg_demo_server.o
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
