TARGETS = new_intr no_echo test_tty_functions demo_SIGWINCH \
          m_isatty m_ttyname m_getpass is_canon
OBJS = new_intr.o no_echo.o tty_functions.o test_tty_functions.o demo_SIGWINCH.o \
       m_isatty.o m_ttyname.o m_getpass.o is_canon.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

new_intr: new_intr.o
new_intr.o: new_intr.c
no_echo: no_echo.o
no_echo.o: no_echo.c
tty_functions.o: tty_functions.c tty_functions.h
test_tty_functions: test_tty_functions.o tty_functions.o
test_tty_functions.o: test_tty_functions.c tty_functions.h
demo_SIGWINCH: demo_SIGWINCH.o
demo_SIGWINCH.o: demo_SIGWINCH.c
m_isatty: m_isatty.o
m_isatty.o: m_isatty.c
m_ttyname: m_ttyname.o
m_ttyname.o: m_ttyname.c
m_getpass: m_getpass.o
m_getpass.o: m_getpass.c
is_canon: is_canon.o
is_canon.o: is_canon.c
