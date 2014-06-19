TARGETS = idshow m_initgroups
OBJS = idshow.o m_initgroups.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

idshow: idshow.o
idshow.o: idshow.c
m_initgroups: m_initgroups.o
m_initgroups.o: m_initgroups.c
