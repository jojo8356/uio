#include "utest.h"
#include "uio.h"

static void test_basename_simple(void)
{
    char buf[256];
    uio_basename("/home/user/file.txt", buf, sizeof(buf));
    ASSERT(strcmp(buf, "file.txt") == 0, "basename");
}

static void test_basename_no_dir(void)
{
    char buf[256];
    uio_basename("file.txt", buf, sizeof(buf));
    ASSERT(strcmp(buf, "file.txt") == 0, "basename no dir");
}

static void test_dirname_simple(void)
{
    char buf[256];
    uio_dirname("/home/user/file.txt", buf, sizeof(buf));
    ASSERT(strcmp(buf, "/home/user") == 0, "dirname");
}

static void test_dirname_no_dir(void)
{
    char buf[256];
    uio_dirname("file.txt", buf, sizeof(buf));
    ASSERT(strcmp(buf, ".") == 0, "dirname no dir returns .");
}

static void test_extension(void)
{
    char buf[64];
    uio_extension("/home/user/file.txt", buf, sizeof(buf));
    ASSERT(strcmp(buf, "txt") == 0, "extension txt");
}

static void test_extension_none(void)
{
    char buf[64];
    uio_extension("/home/user/file", buf, sizeof(buf));
    ASSERT(buf[0] == '\0', "no extension");
}

static void test_extension_tar_gz(void)
{
    char buf[64];
    uio_extension("archive.tar.gz", buf, sizeof(buf));
    ASSERT(strcmp(buf, "gz") == 0, "extension gz");
}

static void test_join_path(void)
{
    char buf[256];
    uio_join_path("/home", "file.txt", buf, sizeof(buf));
#ifdef _WIN32
    ASSERT(strcmp(buf, "/home\\file.txt") == 0, "join_path");
#else
    ASSERT(strcmp(buf, "/home/file.txt") == 0, "join_path");
#endif
}

static void test_join_path_trailing_slash(void)
{
    char buf[256];
    uio_join_path("/home/", "file.txt", buf, sizeof(buf));
    ASSERT(strcmp(buf, "/home/file.txt") == 0, "no double slash");
}

static void test_join_path3(void)
{
    char buf[256];
    uio_join_path3("/home", "user", "file.txt", buf, sizeof(buf));
#ifdef _WIN32
    ASSERT(strcmp(buf, "/home\\user\\file.txt") == 0, "join_path3");
#else
    ASSERT(strcmp(buf, "/home/user/file.txt") == 0, "join_path3");
#endif
}

static void test_realpath(void)
{
    uio_write("build/test_rp.txt", "x");
    char buf[PATH_MAX];
    uio_realpath_buf("build/test_rp.txt", buf, sizeof(buf));
    ASSERT(strlen(buf) > 0, "realpath non-empty");
#ifdef _WIN32
    ASSERT(isalpha((unsigned char)buf[0]) && buf[1] == ':', "realpath is absolute");
#else
    ASSERT(buf[0] == '/', "realpath is absolute");
#endif
    uio_remove_file("build/test_rp.txt");
}
