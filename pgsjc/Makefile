TARGETS = t_pgrp t_setsid t_ctermid catch_SIGHUP disc_SIGHUP \
          job_mon handling_SIGTSTP orphaned_pgrp_SIGHUP \
          t_killpg_bad t_killpg_good t_setpgrp_exec t_pgrl_setsid
OBJS = t_pgrp.o t_setsid.o t_ctermid.o catch_SIGHUP.o disc_SIGHUP.o \
       job_mon.o handling_SIGTSTP.o orphaned_pgrp_SIGHUP.o \
       t_killpg_bad.o t_killpg_good.o t_setpgrp_exec.o t_pgrl_setsid.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_pgrp: t_pgrp.o
t_pgrp.o: t_pgrp.c
t_setsid: t_setsid.o
t_setsid.o: t_setsid.c
t_ctermid: t_ctermid.o
t_ctermid.o: t_ctermid.c
catch_SIGHUP: catch_SIGHUP.o
catch_SIGHUP.o: catch_SIGHUP.c
disc_SIGHUP: disc_SIGHUP.o
disc_SIGHUP.o: disc_SIGHUP.c
job_mon: job_mon.o
job_mon.o: job_mon.c
handling_SIGTSTP: handling_SIGTSTP.o
handling_SIGTSTP.o: handling_SIGTSTP.c
orphaned_pgrp_SIGHUP: orphaned_pgrp_SIGHUP.o
orphaned_pgrp_SIGHUP.o: orphaned_pgrp_SIGHUP.c
t_killpg_bad: t_killpg_bad.o
t_killpg_bad.o: t_killpg_bad.c
t_killpg_good: t_killpg_good.o
t_killpg_good.o: t_killpg_good.c
t_setpgrp_exec: t_setpgrp_exec.o
t_setpgrp_exec.o: t_setpgrp_exec.c
t_pgrl_setsid: t_pgrl_setsid.o
t_pgrl_setsid.o: t_pgrl_setsid.c
