TARGETS = dump_utmpx t_getlogin utmpx_login view_lastlog m_getlogin
OBJS = dump_utmpx.o t_getlogin.o utmpx_login.o view_lastlog.o m_getlogin.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

dump_utmpx: dump_utmpx.o
dump_utmpx.o: dump_utmpx.c
t_getlogin: t_getlogin.o
t_getlogin.o: t_getlogin.c
utmpx_login: utmpx_login.o
utmpx_login.o: utmpx_login.c
view_lastlog: view_lastlog.o
view_lastlog.o: view_lastlog.c
m_getlogin: m_getlogin.o
m_getlogin.o: m_getlogin.c
