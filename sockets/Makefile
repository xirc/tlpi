TARGETS = us_xfr_sv us_xfr_cl ud_ucase_sv ud_ucase_cl ud_dgram \
          us_xfr2_sv us_xfr2_cl us_seqnum_server us_seqnum_client ud_dgram3 \
          byteorder i6d_ucase_sv i6d_ucase_cl is_seqnum_sv is_seqnum_cl \
          t_inet_sockets t_inet_aton_ntoa t_gethostbyname \
          t_gethostbyaddr t_getservbyname t_getservbyport \
          t_readline2 is_seqnum_sv2 is_seqnum_cl2
OBJS = us_xfr_sv.o us_xfr_cl.o ud_ucase_sv.o ud_ucase_cl.o ud_dgram.o \
       us_xfr2_sv.o us_xfr2_cl.o us_seqnum_server.o us_seqnum_client.o ud_dgram3.o \
       byteorder.o read_line.o i6d_ucase_sv.o i6d_ucase_cl.o is_seqnum_sv.o is_seqnum_cl.o \
       t_inet_sockets.o inet_sockets.o t_inet_aton_ntoa.o t_gethostbyname.o \
       t_gethostbyaddr.o t_getservbyname.o t_getservbyport.o \
       t_readline2.o readline2.o is_seqnum_sv2.o is_seqnum_cl2.o

CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS) $(OBJS)
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
us_xfr2_sv: us_xfr2_sv.o
us_xfr2_sv.o: us_xfr2_sv.c us_xfr2.h
us_xfr2_cl: us_xfr2_cl.o
us_xfr2_cl.o: us_xfr2_cl.c us_xfr2.h
us_seqnum_server: us_seqnum_server.o
us_seqnum_server.o: us_seqnum_server.c us_seqnum.h
us_seqnum_client: us_seqnum_client.o
us_seqnum_client.o: us_seqnum_client.c us_seqnum.h
ud_dgram3: ud_dgram3.o
ud_dgram3.o: ud_dgram3.c
byteorder: byteorder.o
byteorder.o: byteorder.c
read_line.o: read_line.c read_line.h
i6d_ucase_sv: i6d_ucase_sv.o
i6d_ucase_sv.o: i6d_ucase_sv.c i6d_ucase.h
i6d_ucase_cl: i6d_ucase_cl.o
i6d_ucase_cl.o: i6d_ucase_cl.c i6d_ucase.h
is_seqnum_sv: is_seqnum_sv.o read_line.o
is_seqnum_sv.o: is_seqnum_sv.c is_seqnum.h
is_seqnum_cl: is_seqnum_cl.o read_line.o
is_seqnum_cl.o: is_seqnum_cl.c is_seqnum.h
t_inet_sockets: t_inet_sockets.o inet_sockets.o
t_inet_sockets.o: t_inet_sockets.c inet_sockets.h
inet_sockets.o: inet_sockets.c inet_sockets.h
t_inet_aton_ntoa: t_inet_aton_ntoa.o
t_inet_aton_ntoa.o: t_inet_aton_ntoa.c
t_gethostbyname: t_gethostbyname.o
t_gethostbyname.o: t_gethostbyname.c
t_gethostbyaddr: t_gethostbyaddr.o
t_gethostbyaddr.o: t_gethostbyaddr.c
t_getservbyname: t_getservbyname.o
t_getservbyname.o: t_getservbyname.c
t_getservbyport: t_getservbyport.o
t_getservbyport.o: t_getservbyport.c
t_readline2: t_readline2.o readline2.o
t_readline2.o: t_readline2.c readline2.h
readline2.o: readline2.c readline2.h
is_seqnum_sv2: is_seqnum_sv2.o readline2.o
is_seqnum_sv2.o: is_seqnum_sv2.c readline2.h
is_seqnum_cl2: is_seqnum_cl2.o readline2.o
is_seqnum_cl2.o: is_seqnum_cl2.c readline2.h
