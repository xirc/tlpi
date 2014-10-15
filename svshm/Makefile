TARGETS = svshm_xfr_writer svshm_xfr_reader svshm_create \
          svshm_rm svshm_attach
OBJS = svshm_xfr_writer.o svshm_xfr_reader.o binary_sems.o svshm_create.o \
       svshm_rm.o svshm_attach.o
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
