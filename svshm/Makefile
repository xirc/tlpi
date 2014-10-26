TARGETS = svshm_xfr_writer svshm_xfr_reader svshm_create \
          svshm_rm svshm_attach svshm_lock svshm_unlock \
          svshm_info svshm_mon \
          svshm_xfr_writer2 svshm_xfr_reader2 \
          svshm_xfr_reader_wrong \
          t_svshm_dirservice
OBJS = svshm_xfr_writer.o svshm_xfr_reader.o binary_sems.o svshm_create.o \
       svshm_rm.o svshm_attach.o svshm_lock.o svshm_unlock.o \
       svshm_info.o svshm_mon.o \
       svshm_xfr_writer2.o svshm_xfr_reader2.o event_flag.o \
       svshm_xfr_reader_wrong.o \
       t_svshm_dirservice.o svshm_dirservice.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

svshm_xfr_writer: svshm_xfr_writer.o binary_sems.o
svshm_xfr_writer.o: svshm_xfr_writer.c svshm_xfr.h binary_sems.h
svshm_xfr_reader: svshm_xfr_reader.o binary_sems.o
svshm_xfr_reader.o: svshm_xfr_reader.c svshm_xfr.h binary_sems.h
binary_sems.o: binary_sems.c binary_sems.h
svshm_create: svshm_create.o
svshm_create.o: svshm_create.c
svshm_rm: svshm_rm.o
svshm_rm.o: svshm_rm.c
svshm_attach: svshm_attach.o
svshm_attach.o: svshm_attach.c
svshm_lock: svshm_lock.o
svshm_lock.o: svshm_lock.c
svshm_unlock: svshm_unlock.o
svshm_unlock.o: svshm_unlock.c
svshm_info: svshm_info.o
svshm_info.o: svshm_info.c
svshm_mon: svshm_mon.o
svshm_mon.o: svshm_mon.c
svshm_xfr_writer2: svshm_xfr_writer2.o event_flag.o
svshm_xfr_writer2.o: svshm_xfr_writer2.c svshm_xfr.h event_flag.h
svshm_xfr_reader2: svshm_xfr_reader2.o event_flag.o
svshm_xfr_reader2.o: svshm_xfr_reader2.c svshm_xfr.h event_flag.h
svshm_xfr_reader_wrong: svshm_xfr_reader_wrong.o binary_sems.o
svshm_xfr_reader_wrong.o: svshm_xfr_reader_wrong.c svshm_xfr.h binary_sems.h
t_svshm_dirservice: t_svshm_dirservice.o svshm_dirservice.o
t_svshm_dirservice.o: t_svshm_dirservice.c svshm_dirservice.h
svshm_dirservice: svshm_dirservice.o
svshm_dirservice.o: svshm_dirservice.c svshm_dirservice.h
