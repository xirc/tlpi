TARGETS = real_timer timed_read t_nanosleep \
          ptmr_sigev_signal ptmr_sigev_thread demo_timerfd \
          m_alarm t_clock_nanosleep t_posix_timer ptmr_sigev_waitinfo
OBJS = real_timer.o timed_read.o t_nanosleep.o itimerspec_from_string.o \
       ptmr_sigev_signal.o ptmr_sigev_thread.o demo_timerfd.o \
       m_alarm.o t_clock_nanosleep.o t_posix_timer.o ptmr_sigev_waitinfo.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

real_timer: real_timer.o
real_timer.o: real_timer.c
timed_read: timed_read.o
timed_read.o: timed_read.c
t_nanosleep: t_nanosleep.o
t_nanosleep.o: t_nanosleep.c
itimerspec_from_string.o: itimerspec_from_string.c itimerspec_from_string.h
ptmr_sigev_signal: ptmr_sigev_signal.o itimerspec_from_string.o
	$(CC) -lrt -o $@ $^
ptmr_sigev_signal.o: ptmr_sigev_signal.c itimerspec_from_string.h
ptmr_sigev_thread: ptmr_sigev_thread.o itimerspec_from_string.o
	$(CC) -lrt -o $@ $^
ptmr_sigev_thread.o: ptmr_sigev_thread.c itimerspec_from_string.h
demo_timerfd: demo_timerfd.o itimerspec_from_string.o
	$(CC) -lrt -o $@ $^
demo_timerfd.o: demo_timerfd.c itimerspec_from_string.h
m_alarm: m_alarm.o
m_alarm.o: m_alarm.c
t_clock_nanosleep: t_clock_nanosleep.o
	$(CC) -lrt -o $@ $^
t_clock_nanosleep.o: t_clock_nanosleep.c
t_posix_timer: t_posix_timer.o
	$(CC) -lrt -o $@ $^
t_posix_timer.o: t_posix_timer.c
ptmr_sigev_waitinfo: ptmr_sigev_waitinfo.o itimerspec_from_string.o
	$(CC) -lrt -o $@ $^
ptmr_sigev_waitinfo.o: ptmr_sigev_waitinfo.c itimerspec_from_string.h
