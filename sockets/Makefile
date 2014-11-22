TARGETS = us_xfr_sv us_xfr_cl
OBJS = us_xfr_sv.o us_xfr_cl.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

us_xfr_sv: us_xfr_sv.o
us_xfr_sv.o: us_xfr_sv.c us_xfr.h
us_xfr_cl: us_xfr_cl.o
us_xfr_cl.o: us_xfr_cl.c us_xfr.h
