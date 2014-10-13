TARGETS = svsem_demo svsem_mon svsem_setall svsem_bad_init \
          svsem_good_init svsem_op svsem_create svsem_rm
OBJS = svsem_demo.o svsem_mon.o svsem_setall.o svsem_bad_init.o \
       svsem_good_init.o svsem_op.o svsem_create.o svsem_rm.o
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
svsem_setall: svsem_setall.o
svsem_setall.o: svsem_setall.c semun.h
svsem_bad_init: svsem_bad_init.o
svsem_bad_init.o: svsem_bad_init.c semun.h
svsem_good_init: svsem_good_init.o
svsem_good_init.o: svsem_good_init.c semun.h
svsem_op: svsem_op.o
svsem_op.o: svsem_op.c
svsem_create: svsem_create.o
svsem_create.o: svsem_create.c
svsem_rm: svsem_rm.o
svsem_rm.o: svsem_rm.c semun.h
