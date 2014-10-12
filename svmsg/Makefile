TARGETS = svmsg_create svmsg_send svmsg_receive svmsg_rm
OBJS = svmsg_create.o svmsg_send.o svmsg_receive.o svmsg_rm.o
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
svmsg_receive: svmsg_receive.o
svmsg_receive.o: svmsg_receive.c
svmsg_rm: svmsg_rm.o
svmsg_rm.o: svmsg_rm.c
