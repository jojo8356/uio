#include "utest.h"
#include "uio.h"

static void test_exists_file(void)
{
    uio_write("build/test_ex.txt", "x");
    ASSERT(uio_exists("build/test_ex.txt") == 1, "file exists");
    uio_remove_file("build/test_ex.txt");
}

static void test_exists_nonexistent(void)
{
    ASSERT(uio_exists("build/no_such_file_xyz.txt") == 0, "does not exist");
}

static void test_is_file(void)
{
    uio_write("build/test_if.txt", "x");
    ASSERT(uio_is_file("build/test_if.txt") == 1, "is a file");
    ASSERT(uio_is_file("build") == 0, "dir is not a file");
    uio_remove_file("build/test_if.txt");
}

static void test_is_dir(void)
{
    ASSERT(uio_is_dir("build") == 1, "build is a dir");
    uio_write("build/test_id.txt", "x");
    ASSERT(uio_is_dir("build/test_id.txt") == 0, "file is not a dir");
    uio_remove_file("build/test_id.txt");
}

static void test_filesize(void)
{
    uio_write("build/test_fs.txt", "ABCDE");
    long long sz = uio_filesize("build/test_fs.txt");
    ASSERT(sz == 5, "filesize should be 5");
    uio_remove_file("build/test_fs.txt");
}

static void test_filesize_nonexistent(void)
{
    long long sz = uio_filesize("build/nope.txt");
    ASSERT(sz == -1, "nonexistent filesize is -1");
}

static void test_copy(void)
{
    uio_write("build/test_cp_src.txt", "copy me");
    int ok = uio_copy("build/test_cp_src.txt", "build/test_cp_dst.txt");
    ASSERT(ok == 1, "copy succeeds");
    char *c = uio_read("build/test_cp_dst.txt");
    ASSERT(strcmp(c, "copy me") == 0, "copy content matches");
    free(c);
    uio_remove_file("build/test_cp_src.txt");
    uio_remove_file("build/test_cp_dst.txt");
}

static void test_move(void)
{
    uio_write("build/test_mv_src.txt", "move me");
    int ok = uio_move("build/test_mv_src.txt", "build/test_mv_dst.txt");
    ASSERT(ok == 1, "move succeeds");
    ASSERT(uio_exists("build/test_mv_src.txt") == 0, "src gone");
    char *c = uio_read("build/test_mv_dst.txt");
    ASSERT(strcmp(c, "move me") == 0, "move content matches");
    free(c);
    uio_remove_file("build/test_mv_dst.txt");
}

static void test_remove(void)
{
    uio_write("build/test_rm.txt", "x");
    ASSERT(uio_exists("build/test_rm.txt") == 1, "file exists before remove");
    uio_remove_file("build/test_rm.txt");
    ASSERT(uio_exists("build/test_rm.txt") == 0, "file gone after remove");
}

static void test_mkdir_p(void)
{
    int ok = uio_mkdir_p("build/test_dir/sub/deep");
    ASSERT(ok == 1, "mkdir_p succeeds");
    ASSERT(uio_is_dir("build/test_dir/sub/deep") == 1, "deep dir exists");
    uio_rmdir_all("build/test_dir");
}

static void test_rmdir_all(void)
{
    uio_mkdir_p("build/test_rm_dir/a/b");
    uio_write("build/test_rm_dir/a/file.txt", "x");
    uio_write("build/test_rm_dir/a/b/file2.txt", "y");
    int ok = uio_rmdir_all("build/test_rm_dir");
    ASSERT(ok == 1, "rmdir_all succeeds");
    ASSERT(uio_exists("build/test_rm_dir") == 0, "dir gone");
}

static void test_listdir(void)
{
    uio_mkdir_p("build/test_ls_dir");
    uio_write("build/test_ls_dir/a.txt", "a");
    uio_write("build/test_ls_dir/b.c", "b");
    uio_write("build/test_ls_dir/c.txt", "c");

    UStrList *all = uio_listdir("build/test_ls_dir");
    ASSERT(ustrlist_size(all) == 3, "3 entries");
    ustrlist_free(all);

    UStrList *txt = uio_listdir_ext("build/test_ls_dir", ".txt");
    ASSERT(ustrlist_size(txt) == 2, "2 .txt files");
    ustrlist_free(txt);

    uio_rmdir_all("build/test_ls_dir");
}

static void test_listdir_recursive(void)
{
    uio_mkdir_p("build/test_lsr/sub");
    uio_write("build/test_lsr/a.txt", "a");
    uio_write("build/test_lsr/sub/b.txt", "b");

    UStrList *all = uio_listdir_recursive("build/test_lsr");
    ASSERT(ustrlist_size(all) == 2, "2 files recursive");
    ustrlist_free(all);

    uio_rmdir_all("build/test_lsr");
}

static void test_tempfile(void)
{
    char path[PATH_MAX];
    int ok = uio_tempfile(path, sizeof(path));
    ASSERT(ok == 1, "tempfile created");
    ASSERT(uio_exists(path) == 1, "tempfile exists");
    uio_remove_file(path);
}

static void test_mtime(void)
{
    uio_write("build/test_mt.txt", "x");
    time_t mt = uio_mtime("build/test_mt.txt");
    ASSERT(mt > 0, "mtime > 0");
    time_t now = time(NULL);
    ASSERT(now - mt < 10, "mtime is recent");
    uio_remove_file("build/test_mt.txt");
}
