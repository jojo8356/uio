#include "utest.h"
#include "uio.h"

/* Helper: redirect stdin from a file with given content */
static void test_set_stdin(const char *content)
{
    uio_write("build/test_stdin.txt", content);
    freopen("build/test_stdin.txt", "r", stdin);
}

static void test_input_string(void)
{
    test_set_stdin("Alice\n");
    const char *r = uio_input(NULL);
    ASSERT(strcmp(r, "Alice") == 0, "input reads Alice");
}

static void test_input_buf(void)
{
    test_set_stdin("Bob\n");
    char buf[64];
    uio_input_buf(NULL, buf, sizeof(buf));
    ASSERT(strcmp(buf, "Bob") == 0, "input_buf reads Bob");
}

static void test_input_int(void)
{
    test_set_stdin("42\n");
    int v = uio_input_int(NULL);
    ASSERT(v == 42, "input_int reads 42");
}

static void test_input_int_retry(void)
{
    test_set_stdin("abc\n42\n");
    int v = uio_input_int(NULL);
    ASSERT(v == 42, "input_int retries on invalid");
}

static void test_input_int_default(void)
{
    test_set_stdin("\n");
    int v = uio_input_int_default(NULL, 8080);
    ASSERT(v == 8080, "input_int_default returns default");
}

static void test_input_int_range(void)
{
    test_set_stdin("25\n15\n");
    int v = uio_input_int_range(NULL, 0, 20);
    ASSERT(v == 15, "input_int_range retries out-of-bounds");
}

static void test_input_double(void)
{
    test_set_stdin("3.14\n");
    double v = uio_input_double(NULL);
    ASSERT(v > 3.13 && v < 3.15, "input_double reads 3.14");
}

static void test_confirm_yes(void)
{
    test_set_stdin("o\n");
    int v = uio_confirm(NULL);
    ASSERT(v == 1, "confirm 'o' returns 1");
}

static void test_confirm_no(void)
{
    test_set_stdin("n\n");
    int v = uio_confirm(NULL);
    ASSERT(v == 0, "confirm 'n' returns 0");
}

static void test_confirm_default(void)
{
    test_set_stdin("\n");
    int v = uio_confirm_default(NULL, 1);
    ASSERT(v == 1, "confirm_default empty returns default");
}

static void test_choose(void)
{
    test_set_stdin("2\n");
    const char *opts[] = {"A", "B", "C", NULL};
    int v = uio_choose(NULL, opts);
    ASSERT(v == 1, "choose '2' returns index 1");
}
