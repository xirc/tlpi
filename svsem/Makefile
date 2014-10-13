TARGETS = svsem_demo svsem_mon
OBJS = svsem_demo.o svsem_mon.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

svsem_demo: svsem_demo.o
svsem_demo.o: svsem_demo.c semun.h
svsem_mon: svsem_mon.o
svsem_mon.o: svsem_mon.c semun.h
