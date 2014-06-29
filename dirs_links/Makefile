TARGETS = t_unlink list_files nftw_dir_tree view_symlink
OBJS = t_unlink.o list_files.o nftw_dir_tree.o view_symlink.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

t_unlink: t_unlink.o
t_unlink.o: t_unlink.c
list_files: list_files.o
list_files.o: list_files.c
nftw_dir_tree: nftw_dir_tree.o
nftw_dir_tree.o: nftw_dir_tree.c
view_symlink: view_symlink.o
view_symlink.o: view_symlink.c
