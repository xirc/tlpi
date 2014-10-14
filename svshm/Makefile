TARGETS = svshm_xfr_writer svshm_xfr_reader
OBJS = svshm_xfr_writer.o svshm_xfr_reader.o binary_sems.o
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
