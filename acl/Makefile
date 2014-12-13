TARGETS = acl_view acl_vperm
OBJS = acl_view.o acl_vperm.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -lacl
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

acl_view: acl_view.o
acl_view.o: acl_view.c
acl_vperm: acl_vperm.o
acl_vperm.o: acl_vperm.c
