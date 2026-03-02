#include "utest.h"
#include "uio.h"

static void test_readlines_basic(void)
{
    uio_write("build/test_rl.txt", "aaa\nbbb\nccc\n");
    UStrList *sl = uio_readlines("build/test_rl.txt");
    ASSERT(sl != NULL, "readlines non-NULL");
    ASSERT(ustrlist_size(sl) == 3, "3 lines");
    ASSERT(strcmp(ustrlist_get(sl, 0), "aaa") == 0, "line 0");
    ASSERT(strcmp(ustrlist_get(sl, 1), "bbb") == 0, "line 1");
    ASSERT(strcmp(ustrlist_get(sl, 2), "ccc") == 0, "line 2");
    ustrlist_free(sl);
    uio_remove_file("build/test_rl.txt");
}

static void test_readlines_empty(void)
{
    uio_write("build/test_rle.txt", "");
    UStrList *sl = uio_readlines("build/test_rle.txt");
    ASSERT(sl != NULL, "readlines empty non-NULL");
    ASSERT(ustrlist_size(sl) == 0, "0 lines");
    ustrlist_free(sl);
    uio_remove_file("build/test_rle.txt");
}

static void test_readlines_count(void)
{
    uio_write("build/test_rlc.txt", "a\nb\nc\n");
    int count;
    UStrList *sl = uio_readlines_count("build/test_rlc.txt", &count);
    ASSERT(count == 3, "count should be 3");
    ustrlist_free(sl);
    uio_remove_file("build/test_rlc.txt");
}

static void test_readlines_no_trailing_newline(void)
{
    uio_write("build/test_rln.txt", "aaa\nbbb");
    UStrList *sl = uio_readlines("build/test_rln.txt");
    ASSERT(ustrlist_size(sl) == 2, "2 lines without trailing newline");
    ASSERT(strcmp(ustrlist_get(sl, 1), "bbb") == 0, "last line ok");
    ustrlist_free(sl);
    uio_remove_file("build/test_rln.txt");
}

static void test_line_reader(void)
{
    uio_write("build/test_lr.txt", "one\ntwo\nthree\n");
    ULineReader r;
    ASSERT(uio_line_open(&r, "build/test_lr.txt") == 1, "open ok");

    char *line = uio_line_next(&r);
    ASSERT(line != NULL && strcmp(line, "one") == 0, "line 1");
    line = uio_line_next(&r);
    ASSERT(line != NULL && strcmp(line, "two") == 0, "line 2");
    line = uio_line_next(&r);
    ASSERT(line != NULL && strcmp(line, "three") == 0, "line 3");
    line = uio_line_next(&r);
    ASSERT(line == NULL, "EOF");

    uio_line_close(&r);
    uio_remove_file("build/test_lr.txt");
}

static void test_line_reader_long_line(void)
{
    /* Generate a line longer than the initial 256-byte buffer */
    char big[2048];
    memset(big, 'X', sizeof(big) - 1);
    big[sizeof(big) - 1] = '\0';

    uio_writef("build/test_lrl.txt", "%s\n", big);

    ULineReader r;
    uio_line_open(&r, "build/test_lrl.txt");
    char *line = uio_line_next(&r);
    ASSERT(line != NULL, "long line read");
    if (line) ASSERT(strlen(line) == sizeof(big) - 1, "long line length");
    uio_line_close(&r);
    uio_remove_file("build/test_lrl.txt");
}

static void test_readlines_crlf(void)
{
    uio_write("build/test_crlf.txt", "aaa\r\nbbb\r\nccc\r\n");
    UStrList *sl = uio_readlines("build/test_crlf.txt");
    ASSERT(ustrlist_size(sl) == 3, "3 lines with CRLF");
    ASSERT(strcmp(ustrlist_get(sl, 0), "aaa") == 0, "no \\r");
    ustrlist_free(sl);
    uio_remove_file("build/test_crlf.txt");
}
