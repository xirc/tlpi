TARGETS = t_unlink list_files nftw_dir_tree view_symlink t_dirbasename \
          bad_symlink m_realpath list_files2 m_getcwd m_nftw_dir_tree \
          m_file_type_stats m_nftw how_chdir
OBJS = t_unlink.o list_files.o nftw_dir_tree.o view_symlink.o t_dirbasename.o \
       bad_symlink.o m_realpath.o list_files2.o m_getcwd.o m_nftw_dir_tree.o \
       m_file_type_stats.o m_nftw.o how_chdir.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_unlink: t_unlink.o
t_unlink.o: t_unlink.c
list_files: list_files.o
list_files.o: list_files.c
nftw_dir_tree: nftw_dir_tree.o
nftw_dir_tree.o: nftw_dir_tree.c
view_symlink: view_symlink.o
view_symlink.o: view_symlink.c
t_dirbasename: t_dirbasename.o
t_dirbasename.o: t_dirbasename.c
bad_symlink: bad_symlink.o
bad_symlink.o: bad_symlink.c
m_realpath: m_realpath.o
m_realpath.o: m_realpath.c
list_files2: list_files2.o
list_files2.o: list_files2.c
m_getcwd: m_getcwd.o
m_getcwd.o: m_getcwd.c
m_nftw_dir_tree: m_nftw_dir_tree.o
m_nftw_dir_tree.o: m_nftw_dir_tree.c
m_file_type_stats: m_file_type_stats.o
m_file_type_stats.o: m_file_type_stats.c
m_nftw: m_nftw.o
m_nftw.o: m_nftw.c
how_chdir: how_chdir.o
how_chdir.o: how_chdir.c
