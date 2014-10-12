TARGETS = svmsg_create svmsg_send
OBJS = svmsg_create.o svmsg_send.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

svmsg_create: svmsg_create.o
svmsg_create.o: svmsg_create.c
svmsg_send: svmsg_send.o
svmsg_send.o: svmsg_send.c
