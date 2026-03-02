#include "utest.h"
#include "uio.h"

static void test_write_basic(void)
{
    int ok = uio_write("build/test_w.txt", "hello");
    ASSERT(ok == 1, "write should succeed");
    char *c = uio_read("build/test_w.txt");
    ASSERT(strcmp(c, "hello") == 0, "content should match");
    free(c);
    uio_remove_file("build/test_w.txt");
}

static void test_writef(void)
{
    uio_writef("build/test_wf.txt", "Score: %d/%d", 42, 50);
    char *c = uio_read("build/test_wf.txt");
    ASSERT(strcmp(c, "Score: 42/50") == 0, "formatted content");
    free(c);
    uio_remove_file("build/test_wf.txt");
}

static void test_write_bytes(void)
{
    unsigned char data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    int ok = uio_write_bytes("build/test_wb.dat", data, 4);
    ASSERT(ok == 1, "write_bytes should succeed");
    size_t sz;
    unsigned char *rd = uio_read_bytes("build/test_wb.dat", &sz);
    ASSERT(sz == 4 && rd[0] == 0xDE && rd[3] == 0xEF, "bytes match");
    free(rd);
    uio_remove_file("build/test_wb.dat");
}

static void test_append(void)
{
    uio_write("build/test_a.txt", "hello");
    uio_append("build/test_a.txt", " world");
    char *c = uio_read("build/test_a.txt");
    ASSERT(strcmp(c, "hello world") == 0, "appended content");
    free(c);
    uio_remove_file("build/test_a.txt");
}

static void test_appendf(void)
{
    uio_write("build/test_af.txt", "x=");
    uio_appendf("build/test_af.txt", "%d", 42);
    char *c = uio_read("build/test_af.txt");
    ASSERT(strcmp(c, "x=42") == 0, "appendf content");
    free(c);
    uio_remove_file("build/test_af.txt");
}

static void test_writelines(void)
{
    const char *lines[] = {"line 1", "line 2", "line 3", NULL};
    uio_writelines("build/test_wl.txt", lines);
    char *c = uio_read("build/test_wl.txt");
    ASSERT(strcmp(c, "line 1\nline 2\nline 3\n") == 0, "writelines");
    free(c);
    uio_remove_file("build/test_wl.txt");
}

static void test_write_overwrite(void)
{
    uio_write("build/test_ow.txt", "first");
    uio_write("build/test_ow.txt", "second");
    char *c = uio_read("build/test_ow.txt");
    ASSERT(strcmp(c, "second") == 0, "overwrite keeps only last");
    free(c);
    uio_remove_file("build/test_ow.txt");
}
