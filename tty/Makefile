TARGETS = new_intr no_echo
OBJS = new_intr.o no_echo.o
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
