#include "utest.h"
#include "uio.h"

static void test_read_text(void)
{
    uio_write("build/test_read.txt", "Hello, World!");
    char *c = uio_read("build/test_read.txt");
    ASSERT(c != NULL, "uio_read should return non-NULL");
    if (c) { ASSERT(strcmp(c, "Hello, World!") == 0, "content should match"); free(c); }
    uio_remove_file("build/test_read.txt");
}

static void test_read_empty(void)
{
    uio_write("build/test_empty.txt", "");
    char *c = uio_read("build/test_empty.txt");
    ASSERT(c != NULL, "empty file should return non-NULL");
    if (c) { ASSERT(c[0] == '\0', "empty file should return empty string"); free(c); }
    uio_remove_file("build/test_empty.txt");
}

static void test_read_nonexistent(void)
{
    char *c = uio_read("build/no_such_file.txt");
    ASSERT(c == NULL, "nonexistent file should return NULL");
}

static void test_read_len(void)
{
    uio_write("build/test_len.txt", "ABCDE");
    size_t len;
    char *c = uio_read_len("build/test_len.txt", &len);
    ASSERT(c != NULL, "read_len should return non-NULL");
    if (c) { ASSERT(len == 5, "len should be 5"); free(c); }
    uio_remove_file("build/test_len.txt");
}

static void test_read_bytes(void)
{
    unsigned char data[] = {0x00, 0x01, 0xFF, 0x42};
    uio_write_bytes("build/test_bin.dat", data, 4);
    size_t sz;
    unsigned char *rd = uio_read_bytes("build/test_bin.dat", &sz);
    ASSERT(rd != NULL, "read_bytes should return non-NULL");
    if (rd) {
        ASSERT(sz == 4, "size should be 4");
        ASSERT(rd[0] == 0x00 && rd[2] == 0xFF && rd[3] == 0x42, "bytes match");
        free(rd);
    }
    uio_remove_file("build/test_bin.dat");
}

static void test_read_head(void)
{
    uio_write("build/test_head.txt", "ABCDEFGHIJ");
    char *c = uio_read_head("build/test_head.txt", 5);
    ASSERT(c != NULL, "read_head non-NULL");
    if (c) { ASSERT(strcmp(c, "ABCDE") == 0, "first 5 bytes"); free(c); }
    uio_remove_file("build/test_head.txt");
}

static void test_read_tail(void)
{
    uio_write("build/test_tail.txt", "ABCDEFGHIJ");
    char *c = uio_read_tail("build/test_tail.txt", 5);
    ASSERT(c != NULL, "read_tail non-NULL");
    if (c) { ASSERT(strcmp(c, "FGHIJ") == 0, "last 5 bytes"); free(c); }
    uio_remove_file("build/test_tail.txt");
}
