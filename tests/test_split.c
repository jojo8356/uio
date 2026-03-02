#include "utest.h"
#include "uio.h"

static void test_split_basic(void)
{
    UStrList *sl = uio_split("a,b,c", ",");
    ASSERT(ustrlist_size(sl) == 3, "3 parts");
    ASSERT(strcmp(ustrlist_get(sl, 0), "a") == 0, "part 0");
    ASSERT(strcmp(ustrlist_get(sl, 1), "b") == 0, "part 1");
    ASSERT(strcmp(ustrlist_get(sl, 2), "c") == 0, "part 2");
    ustrlist_free(sl);
}

static void test_split_empty_fields(void)
{
    UStrList *sl = uio_split("a,,b", ",");
    ASSERT(ustrlist_size(sl) == 3, "3 parts with empty");
    ASSERT(strcmp(ustrlist_get(sl, 1), "") == 0, "empty field preserved");
    ustrlist_free(sl);
}

static void test_split_delim_start(void)
{
    UStrList *sl = uio_split(",a,b", ",");
    ASSERT(ustrlist_size(sl) == 3, "3 parts");
    ASSERT(strcmp(ustrlist_get(sl, 0), "") == 0, "empty first");
    ustrlist_free(sl);
}

static void test_split_delim_end(void)
{
    UStrList *sl = uio_split("a,b,", ",");
    ASSERT(ustrlist_size(sl) == 3, "3 parts");
    ASSERT(strcmp(ustrlist_get(sl, 2), "") == 0, "empty last");
    ustrlist_free(sl);
}

static void test_split_no_delim(void)
{
    UStrList *sl = uio_split("hello", ",");
    ASSERT(ustrlist_size(sl) == 1, "1 part");
    ASSERT(strcmp(ustrlist_get(sl, 0), "hello") == 0, "whole string");
    ustrlist_free(sl);
}

static void test_split_empty_string(void)
{
    UStrList *sl = uio_split("", ",");
    ASSERT(ustrlist_size(sl) == 1, "1 part (empty)");
    ASSERT(strcmp(ustrlist_get(sl, 0), "") == 0, "empty string");
    ustrlist_free(sl);
}

static void test_split_multichar_delim(void)
{
    UStrList *sl = uio_split("a::b::c", "::");
    ASSERT(ustrlist_size(sl) == 3, "3 parts");
    ASSERT(strcmp(ustrlist_get(sl, 1), "b") == 0, "part 1");
    ustrlist_free(sl);
}

static void test_splitn(void)
{
    UStrList *sl = uio_splitn("a,b,c,d,e", ",", 3);
    ASSERT(ustrlist_size(sl) == 3, "3 parts with limit");
    ASSERT(strcmp(ustrlist_get(sl, 0), "a") == 0, "part 0");
    ASSERT(strcmp(ustrlist_get(sl, 2), "c,d,e") == 0, "rest unsplit");
    ustrlist_free(sl);
}

static void test_words(void)
{
    UStrList *sl = uio_words("  hello   world   test  ");
    ASSERT(ustrlist_size(sl) == 3, "3 words");
    ASSERT(strcmp(ustrlist_get(sl, 0), "hello") == 0, "word 0");
    ASSERT(strcmp(ustrlist_get(sl, 1), "world") == 0, "word 1");
    ASSERT(strcmp(ustrlist_get(sl, 2), "test") == 0, "word 2");
    ustrlist_free(sl);
}

static void test_split_inplace(void)
{
    char buf[] = "hello,world,test";
    const char *parts[16];
    int count = uio_split_inplace(buf, ",", parts, 16);
    ASSERT(count == 3, "3 parts");
    ASSERT(strcmp(parts[0], "hello") == 0, "part 0");
    ASSERT(strcmp(parts[1], "world") == 0, "part 1");
    ASSERT(strcmp(parts[2], "test") == 0, "part 2");
}

static void test_join(void)
{
    UStrList *sl = uio_split("hello,world,test", ",");
    char *joined = uio_join(sl, ", ");
    ASSERT(strcmp(joined, "hello, world, test") == 0, "joined string");
    free(joined);
    ustrlist_free(sl);
}

static void test_join_buf(void)
{
    UStrList *sl = uio_split("a,b,c", ",");
    char buf[32];
    uio_join_buf(sl, "-", buf, sizeof(buf));
    ASSERT(strcmp(buf, "a-b-c") == 0, "join_buf");
    ustrlist_free(sl);
}
