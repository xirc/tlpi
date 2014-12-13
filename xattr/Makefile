TARGETS = xattr_view setfattr
OBJS = xattr_view.o setfattr.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

xattr_view: xattr_view.o
xattr_view.o: xattr_view.c
setfattr: setfattr.o
setfattr.o: setfattr.c
