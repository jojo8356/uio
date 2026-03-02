#include "utest.h"
#include "uio.h"

static void test_csv_basic(void)
{
    uio_write("build/test.csv", "a,b,c\n1,2,3\n4,5,6\n");
    UCSV csv;
    ASSERT(uio_csv_open(&csv, "build/test.csv", ',') == 1, "csv open");

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row 1"); if (!row) goto done;
    ASSERT(csv.col_count == 3, "3 cols");
    ASSERT(strcmp(row[0], "a") == 0, "field a");

    row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row 2"); if (!row) goto done;
    ASSERT(strcmp(row[0], "1") == 0, "field 1");

    row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row 3"); if (!row) goto done;
    ASSERT(strcmp(row[0], "4") == 0, "field 4");

    row = uio_csv_next(&csv);
    ASSERT(row == NULL, "EOF");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test.csv");
}

static void test_csv_quoted(void)
{
    uio_write("build/test_q.csv", "a,\"hello, world\",c\n");
    UCSV csv;
    uio_csv_open(&csv, "build/test_q.csv", ',');

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row exists"); if (!row) goto done;
    ASSERT(csv.col_count == 3, "3 cols");
    ASSERT(strcmp(row[1], "hello, world") == 0, "quoted field with comma");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test_q.csv");
}

static void test_csv_escaped_quote(void)
{
    uio_write("build/test_eq.csv", "a,\"he said \"\"ok\"\"\",c\n");
    UCSV csv;
    uio_csv_open(&csv, "build/test_eq.csv", ',');

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row exists"); if (!row) goto done;
    ASSERT(strcmp(row[1], "he said \"ok\"") == 0, "escaped quotes");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test_eq.csv");
}

static void test_csv_header(void)
{
    uio_write("build/test_h.csv", "nom,age,ville\nAlice,30,Paris\n");
    UCSV csv;
    ASSERT(uio_csv_open_header(&csv, "build/test_h.csv", ',') == 1, "open header");
    ASSERT(csv.header_count == 3, "3 headers");
    ASSERT(strcmp(csv.headers[0], "nom") == 0, "header 0");

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "data row"); if (!row) goto done;
    ASSERT(strcmp(row[0], "Alice") == 0, "Alice");

    const char *ville = uio_csv_get(&csv, row, "ville");
    ASSERT(ville != NULL, "ville found"); if (!ville) goto done;
    ASSERT(strcmp(ville, "Paris") == 0, "ville = Paris");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test_h.csv");
}

static void test_csv_semicolon(void)
{
    uio_write("build/test_sc.csv", "a;b;c\n1;2;3\n");
    UCSV csv;
    uio_csv_open(&csv, "build/test_sc.csv", ';');

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row exists"); if (!row) goto done;
    ASSERT(csv.col_count == 3, "3 cols with ;");
    ASSERT(strcmp(row[1], "b") == 0, "field b");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test_sc.csv");
}

static void test_csv_empty_file(void)
{
    uio_write("build/test_ce.csv", "");
    UCSV csv;
    uio_csv_open(&csv, "build/test_ce.csv", ',');

    char **row = uio_csv_next(&csv);
    ASSERT(row == NULL, "empty CSV returns NULL");

    uio_csv_close(&csv);
    uio_remove_file("build/test_ce.csv");
}

static void test_csv_writer(void)
{
    UCSVWriter w;
    ASSERT(uio_csv_writer_open(&w, "build/test_cw.csv", ',') == 1, "writer open");

    const char *h[] = {"nom", "age", NULL};
    uio_csv_write_row(&w, h);

    const char *r1[] = {"Alice", "30", NULL};
    uio_csv_write_row(&w, r1);

    const char *r2[] = {"hello, world", "\"quoted\"", NULL};
    uio_csv_write_row(&w, r2);

    uio_csv_writer_close(&w);

    /* Verify by reading back */
    UCSV csv;
    uio_csv_open_header(&csv, "build/test_cw.csv", ',');

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row 1"); if (!row) goto done;
    ASSERT(strcmp(row[0], "Alice") == 0, "Alice");

    row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row 2"); if (!row) goto done;
    ASSERT(strcmp(row[0], "hello, world") == 0, "quoted roundtrip");
    ASSERT(strcmp(row[1], "\"quoted\"") == 0, "escaped quote roundtrip");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test_cw.csv");
}

static void test_csv_newline_in_field(void)
{
    uio_write("build/test_nl.csv", "a,\"line1\nline2\",c\n");
    UCSV csv;
    uio_csv_open(&csv, "build/test_nl.csv", ',');

    char **row = uio_csv_next(&csv);
    ASSERT(row != NULL, "row exists"); if (!row) goto done;
    ASSERT(csv.col_count == 3, "3 cols");
    ASSERT(strcmp(row[1], "line1\nline2") == 0, "newline in quoted field");

done:
    uio_csv_close(&csv);
    uio_remove_file("build/test_nl.csv");
}
