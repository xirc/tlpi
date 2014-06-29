TARGETS = t_unlink list_files
OBJS = t_unlink.o list_files.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

t_unlink: t_unlink.o
t_unlink.o: t_unlink.c
list_files: list_files.o
list_files.o: list_files.c
