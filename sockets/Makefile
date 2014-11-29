TARGETS = us_xfr_sv us_xfr_cl ud_ucase_sv ud_ucase_cl ud_dgram
OBJS = us_xfr_sv.o us_xfr_cl.o ud_ucase_sv.o ud_ucase_cl.o ud_dgram.o
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
ud_ucase_sv: ud_ucase_sv.o
ud_ucase_sv.o: ud_ucase_sv.c ud_ucase.h
ud_ucase_cl: ud_ucase_cl.o
ud_ucase_cl.o: ud_ucase_cl.c ud_ucase.h
ud_dgram: ud_dgram.o
ud_dgram.o: ud_dgram.c
