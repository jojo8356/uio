#include "utest.h"
#include "uio.h"

/* Include all test files (single compilation unit) */
#include "test_read.c"
#include "test_write.c"
#include "test_lines.c"
#include "test_split.c"
#include "test_input.c"
#include "test_fs.c"
#include "test_csv.c"
#include "test_path.c"
#include "test_edge.c"

int main(void)
{
    printf("UIO — Full Test Suite\n");
    printf("========================================\n");

    /* Ensure build dir exists for temp files */
    uio_mkdir_p("build");

    /* Read */
    printf("\n--- Read ---\n");
    TEST(test_read_text);
    TEST(test_read_empty);
    TEST(test_read_nonexistent);
    TEST(test_read_len);
    TEST(test_read_bytes);
    TEST(test_read_head);
    TEST(test_read_tail);

    /* Write */
    printf("\n--- Write ---\n");
    TEST(test_write_basic);
    TEST(test_writef);
    TEST(test_write_bytes);
    TEST(test_append);
    TEST(test_appendf);
    TEST(test_writelines);
    TEST(test_write_overwrite);

    /* Lines */
    printf("\n--- Lines ---\n");
    TEST(test_readlines_basic);
    TEST(test_readlines_empty);
    TEST(test_readlines_count);
    TEST(test_readlines_no_trailing_newline);
    TEST(test_line_reader);
    TEST(test_line_reader_long_line);
    TEST(test_readlines_crlf);

    /* Split */
    printf("\n--- Split ---\n");
    TEST(test_split_basic);
    TEST(test_split_empty_fields);
    TEST(test_split_delim_start);
    TEST(test_split_delim_end);
    TEST(test_split_no_delim);
    TEST(test_split_empty_string);
    TEST(test_split_multichar_delim);
    TEST(test_splitn);
    TEST(test_words);
    TEST(test_split_inplace);
    TEST(test_join);
    TEST(test_join_buf);

    /* Input (uses stdin redirection — run before other stdin-dependent tests) */
    printf("\n--- Input ---\n");
    TEST(test_input_string);
    TEST(test_input_buf);
    TEST(test_input_int);
    TEST(test_input_int_retry);
    TEST(test_input_int_default);
    TEST(test_input_int_range);
    TEST(test_input_double);
    TEST(test_confirm_yes);
    TEST(test_confirm_no);
    TEST(test_confirm_default);
    TEST(test_choose);

    /* Filesystem */
    printf("\n--- Filesystem ---\n");
    TEST(test_exists_file);
    TEST(test_exists_nonexistent);
    TEST(test_is_file);
    TEST(test_is_dir);
    TEST(test_filesize);
    TEST(test_filesize_nonexistent);
    TEST(test_copy);
    TEST(test_move);
    TEST(test_remove);
    TEST(test_mkdir_p);
    TEST(test_rmdir_all);
    TEST(test_listdir);
    TEST(test_listdir_recursive);
    TEST(test_tempfile);
    TEST(test_mtime);

    /* CSV */
    printf("\n--- CSV ---\n");
    TEST(test_csv_basic);
    TEST(test_csv_quoted);
    TEST(test_csv_escaped_quote);
    TEST(test_csv_header);
    TEST(test_csv_semicolon);
    TEST(test_csv_empty_file);
    TEST(test_csv_writer);
    TEST(test_csv_newline_in_field);

    /* Paths */
    printf("\n--- Paths ---\n");
    TEST(test_basename_simple);
    TEST(test_basename_no_dir);
    TEST(test_dirname_simple);
    TEST(test_dirname_no_dir);
    TEST(test_extension);
    TEST(test_extension_none);
    TEST(test_extension_tar_gz);
    TEST(test_join_path);
    TEST(test_join_path_trailing_slash);
    TEST(test_join_path3);
    TEST(test_realpath);

    /* Edge Cases */
    printf("\n--- Edge Cases ---\n");
    TEST(test_split_null);
    TEST(test_split_empty_delim);
    TEST(test_join_empty_list);
    TEST(test_many_lines);
    TEST(test_many_split_fields);
    TEST(test_path_with_spaces);
    TEST(test_read_binary_with_null);

    /* Cleanup stdin test file */
    uio_remove_file("build/test_stdin.txt");

    TEST_REPORT();
    return TEST_EXIT_CODE();
}
