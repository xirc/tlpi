TARGETS = t_stat t_chown t_umask t_stat2 eaccess \
          getumask chmod_aprx chattr
OBJS = file_perms.o t_stat.o t_chown.o t_umask.o t_stat2.o eaccess.o \
       getumask.o chmod_aprx.o chattr.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

t_stat: t_stat.o file_perms.o
t_stat.o: t_stat.c file_perms.h
file_perms.o: file_perms.c file_perms.h
t_chown: t_chown.o
t_chown.o: t_chown.c
t_umask: t_umask.o file_perms.o
t_umask.o: t_umask.c file_perms.h
t_stat2: t_stat2.o file_perms.o
t_stat2.o: t_stat2.c file_perms.h
eaccess: eaccess.o
eaccess.o: eaccess.c
getumask: getumask.o
getumask.o: getumask.c
chmod_aprx: chmod_aprx.o
chmod_aprx.o: chmod_aprx.c
chattr: chattr.o
chattr.o: chattr.c
