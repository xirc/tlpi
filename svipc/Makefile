TARGETS = svmsg_demo_server t_ftok m_ftok t_get
OBJS = svmsg_demo_server.o t_ftok.o m_ftok.o t_get.o
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
m_ftok: m_ftok.o
m_ftok.o: m_ftok.c
t_get: t_get.o
t_get.o: t_get.c
