TARGETS = t_getrusage rlimit_nproc \
          getrusage_fork rusage \
          setrlimit_as setrlimit_nice setrlimit_nproc
OBJS = t_getrusage.o rlimit_nproc.o print_rlimit.o \
       getrusage_fork.o print_rusage.o rusage.o \
       setrlimit_as.o setrlimit_nice.o setrlimit_nproc.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_getrusage: t_getrusage.o
t_getrusage.o: t_getrusage.c
rlimit_nproc: rlimit_nproc.o print_rlimit.o
rlimit_nproc.o: rlimit_nproc.c print_rlimit.h
print_rlimit: print_rlimit.o
print_rlimit.o: print_rlimit.c print_rlimit.h
getrusage_fork: getrusage_fork.o print_rusage.o
getrusage_fork.o: getrusage_fork.c print_rusage.h
print_rusage.o: print_rusage.c print_rusage.h
rusage: rusage.o print_rusage.o
rusage.o: rusage.c print_rusage.h
setrlimit_as: setrlimit_as.o
setrlimit_as.o: setrlimit_as.c
setrlimit_nice: setrlimit_nice.o
setrlimit_nice.o: setrlimit_nice.c
setrlimit_nproc: setrlimit_nproc.o
setrlimit_nproc.o: setrlimit_nproc.c
