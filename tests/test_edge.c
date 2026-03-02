#include "utest.h"
#include "uio.h"

static void test_split_null(void)
{
    UStrList *sl = uio_split(NULL, ",");
    ASSERT(sl == NULL, "split NULL returns NULL");
}

static void test_split_empty_delim(void)
{
    UStrList *sl = uio_split("hello", "");
    ASSERT(ustrlist_size(sl) == 1, "empty delim returns whole string");
    ASSERT(strcmp(ustrlist_get(sl, 0), "hello") == 0, "whole string");
    ustrlist_free(sl);
}

static void test_join_empty_list(void)
{
    UStrList *sl = ustrlist_new();
    char *j = uio_join(sl, ", ");
    ASSERT(j != NULL, "join empty non-NULL");
    if (j) {
        ASSERT(j[0] == '\0', "join empty is empty string");
        free(j);
    }
    ustrlist_free(sl);
}

static void test_many_lines(void)
{
    /* Write 1000 lines */
    FILE *f = fopen("build/test_many.txt", "w");
    if (!f) { ASSERT(0, "fopen failed"); return; }
    for (int i = 0; i < 1000; i++) fprintf(f, "line %d\n", i);
    fclose(f);

    UStrList *sl = uio_readlines("build/test_many.txt");
    ASSERT(ustrlist_size(sl) == 1000, "1000 lines");
    ASSERT(strcmp(ustrlist_get(sl, 999), "line 999") == 0, "last line");
    ustrlist_free(sl);
    uio_remove_file("build/test_many.txt");
}

static void test_many_split_fields(void)
{
    /* Build a string with 500 comma-separated fields */
    char big[4096];
    size_t pos = 0;
    for (int i = 0; i < 500; i++) {
        if (i > 0) big[pos++] = ',';
        pos += (size_t)snprintf(big + pos, sizeof(big) - pos, "%d", i);
    }

    UStrList *sl = uio_split(big, ",");
    ASSERT(ustrlist_size(sl) == 500, "500 fields");
    ASSERT(strcmp(ustrlist_get(sl, 0), "0") == 0, "field 0");
    ASSERT(strcmp(ustrlist_get(sl, 499), "499") == 0, "field 499");
    ustrlist_free(sl);
}

static void test_path_with_spaces(void)
{
    uio_mkdir_p("build/test dir");
    uio_write("build/test dir/file.txt", "space test");
    char *c = uio_read("build/test dir/file.txt");
    ASSERT(c != NULL, "read file with spaces in path");
    if (c) {
        ASSERT(strcmp(c, "space test") == 0, "content matches");
        free(c);
    }
    uio_rmdir_all("build/test dir");
}

static void test_read_binary_with_null(void)
{
    unsigned char data[] = {0x41, 0x00, 0x42, 0x00, 0x43};
    uio_write_bytes("build/test_null.bin", data, 5);
    size_t sz;
    unsigned char *rd = uio_read_bytes("build/test_null.bin", &sz);
    ASSERT(sz == 5, "5 bytes read");
    ASSERT(rd[0] == 0x41 && rd[1] == 0x00 && rd[2] == 0x42, "null bytes ok");
    free(rd);
    uio_remove_file("build/test_null.bin");
}
