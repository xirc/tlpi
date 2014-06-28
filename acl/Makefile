TARGETS = acl_view
OBJS = acl_view.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

acl_view: acl_view.o
	$(CC) -lacl -o $@ $^
acl_view.o: acl_view.c
