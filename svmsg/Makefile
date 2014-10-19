TARGETS = svmsg_create svmsg_send svmsg_receive svmsg_rm svmsg_chqbytes \
          svmsg_ls svmsg_file_server svmsg_file_client \
          svmsg_seqnum_server svmsg_seqnum_client \
          svmsg_file_server2 svmsg_file_client2 svmsg_file_client2_evil \
          svmsg_file_client3
OBJS = svmsg_create.o svmsg_send.o svmsg_receive.o svmsg_rm.o svmsg_chqbytes.o \
       svmsg_ls.o svmsg_file_server.o svmsg_file_client.o \
       svmsg_seqnum_server.o svmsg_seqnum_client.o \
       svmsg_file_server2.o svmsg_file_client2.o svmsg_file_client2_evil.o \
       svmsg_file_client3.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

svmsg_create: svmsg_create.o
svmsg_create.o: svmsg_create.c
svmsg_send: svmsg_send.o
svmsg_send.o: svmsg_send.c
svmsg_receive: svmsg_receive.o
svmsg_receive.o: svmsg_receive.c
svmsg_rm: svmsg_rm.o
svmsg_rm.o: svmsg_rm.c
svmsg_chqbytes: svmsg_chqbytes.o
svmsg_chqbytes.o: svmsg_chqbytes.c
svmsg_ls: svmsg_ls.o
svmsg_ls.o: svmsg_ls.c
svmsg_file_server: svmsg_file_server.o
svmsg_file_server.o: svmsg_file_server.c svmsg_file.h
svmsg_file_client: svmsg_file_client.o
svmsg_file_client.o: svmsg_file_client.c svmsg_file.h
svmsg_seqnum_server: svmsg_seqnum_server.o
svmsg_seqnum_server.o: svmsg_seqnum_server.c svmsg_seqnum.h
svmsg_seqnum_client: svmsg_seqnum_client.o
svmsg_seqnum_client.o: svmsg_seqnum_client.c svmsg_seqnum.h
svmsg_file_server2: svmsg_file_server2.o
svmsg_file_server2.o: svmsg_file_server2.c svmsg_file.h
svmsg_file_client2: svmsg_file_client2.o
svmsg_file_client2.o: svmsg_file_client2.c svmsg_file.h
svmsg_file_client2_evil: svmsg_file_client2_evil.o
svmsg_file_client2_evil.o: svmsg_file_client2_evil.c svmsg_file.h
svmsg_file_client3: svmsg_file_client3.o
svmsg_file_client3.o: svmsg_file_client3.c svmsg_file.h
