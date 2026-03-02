/*
 * UIO — Universal I/O (C)
 * Simplified file I/O, input, split, filesystem, CSV
 *
 * Depends on: UList (ulist.h) for UStrList
 *
 * Usage:
 *   #include "uio.h"          // declarations only
 *
 * Header-only (stb-style):
 *   #define UIO_IMPLEMENTATION
 *   #include "uio.h"          // in exactly ONE .c file
 */

#ifndef UIO_H
#define UIO_H

/* Feature test macros — must come before any includes */
#if !defined(_WIN32)
  #if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
  #endif
  #ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
  #endif
#endif
#if defined(__APPLE__) && !defined(_DARWIN_C_SOURCE)
  #define _DARWIN_C_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include <direct.h>
  #include <io.h>
  #include <sys/stat.h>
  #define UIO_SEP '\\'
  #define UIO_SEP_STR "\\"
#else
  #include <dirent.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <termios.h>
  #define UIO_SEP '/'
  #define UIO_SEP_STR "/"
#endif

#ifndef PATH_MAX
  #ifdef MAX_PATH
    #define PATH_MAX MAX_PATH
  #else
    #define PATH_MAX 4096
  #endif
#endif

/* UList dependency */
#include "ulist.h"

/* ============================================================
 * Types
 * ============================================================ */

typedef struct {
    FILE *file;
    char *buffer;
    size_t buf_size;
    size_t line_len;
} ULineReader;

typedef struct {
    FILE *file;
    char *line_buf;
    size_t line_buf_size;
    char *field_buf;
    size_t field_buf_size;
    char **fields;
    int field_cap;
    int col_count;
    int row_number;
    char delimiter;
    int has_header;
    char **headers;
    int header_count;
} UCSV;

typedef struct {
    FILE *file;
    char delimiter;
} UCSVWriter;

/* ============================================================
 * File Read
 * ============================================================ */

char          *uio_read(const char *path);
char          *uio_read_len(const char *path, size_t *out_len);
unsigned char *uio_read_bytes(const char *path, size_t *out_size);
char          *uio_read_head(const char *path, size_t n);
char          *uio_read_tail(const char *path, size_t n);

/* ============================================================
 * Line Reading
 * ============================================================ */

UStrList *uio_readlines(const char *path);
UStrList *uio_readlines_count(const char *path, int *out_count);

int    uio_line_open(ULineReader *r, const char *path);
char  *uio_line_next(ULineReader *r);
void   uio_line_close(ULineReader *r);

/* ============================================================
 * File Write
 * ============================================================ */

int uio_write(const char *path, const char *content);
int uio_writef(const char *path, const char *fmt, ...);
int uio_write_bytes(const char *path, const void *data, size_t size);
int uio_append(const char *path, const char *content);
int uio_appendf(const char *path, const char *fmt, ...);
int uio_writelines(const char *path, const char **lines);

/* ============================================================
 * Split / Tokenize
 * ============================================================ */

UStrList *uio_split(const char *str, const char *delim);
UStrList *uio_splitn(const char *str, const char *delim, int max);
UStrList *uio_words(const char *str);
int       uio_split_inplace(char *buf, const char *delim,
                             const char **parts, int max_parts);

char *uio_join(const UStrList *sl, const char *sep);
void  uio_join_buf(const UStrList *sl, const char *sep,
                    char *buf, size_t size);

/* ============================================================
 * User Input
 * ============================================================ */

const char *uio_input(const char *prompt);
void        uio_input_buf(const char *prompt, char *buf, size_t size);

int    uio_input_int(const char *prompt);
int    uio_input_int_default(const char *prompt, int def);
int    uio_input_int_range(const char *prompt, int lo, int hi);

double uio_input_double(const char *prompt);
double uio_input_double_default(const char *prompt, double def);

int    uio_confirm(const char *prompt);
int    uio_confirm_default(const char *prompt, int def);

int    uio_choose(const char *prompt, const char **options);

void   uio_password(const char *prompt, char *buf, size_t size);

/* ============================================================
 * Filesystem
 * ============================================================ */

int       uio_exists(const char *path);
int       uio_is_file(const char *path);
int       uio_is_dir(const char *path);
long long uio_filesize(const char *path);
time_t    uio_mtime(const char *path);

int uio_copy(const char *src, const char *dst);
int uio_move(const char *src, const char *dst);
int uio_remove_file(const char *path);
int uio_mkdir_p(const char *path);
int uio_rmdir_empty(const char *path);
int uio_rmdir_all(const char *path);

int uio_tempfile(char *buf, size_t size);
int uio_tempdir(char *buf, size_t size);

/* ============================================================
 * Directory Listing
 * ============================================================ */

UStrList *uio_listdir(const char *path);
UStrList *uio_listdir_ext(const char *path, const char *ext);
UStrList *uio_listdir_recursive(const char *path);

/* ============================================================
 * Paths
 * ============================================================ */

void uio_basename(const char *path, char *buf, size_t size);
void uio_dirname(const char *path, char *buf, size_t size);
void uio_extension(const char *path, char *buf, size_t size);
void uio_join_path(const char *a, const char *b, char *buf, size_t size);
void uio_join_path3(const char *a, const char *b, const char *c,
                     char *buf, size_t size);
void uio_realpath_buf(const char *path, char *buf, size_t size);

/* ============================================================
 * CSV
 * ============================================================ */

int         uio_csv_open(UCSV *csv, const char *path, char delimiter);
int         uio_csv_open_header(UCSV *csv, const char *path, char delimiter);
char      **uio_csv_next(UCSV *csv);
const char *uio_csv_get(const UCSV *csv, char **row, const char *col);
void        uio_csv_close(UCSV *csv);

int  uio_csv_writer_open(UCSVWriter *w, const char *path, char delimiter);
void uio_csv_write_row(UCSVWriter *w, const char **fields);
void uio_csv_writer_close(UCSVWriter *w);

/* ============================================================
 * IMPLEMENTATION
 * ============================================================ */

#ifdef UIO_IMPLEMENTATION

/* ------------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------------ */

static void uio__strlist_add_n(UStrList *sl, const char *s, size_t n)
{
    char *tmp = (char *)malloc(n + 1);
    if (!tmp) return;
    memcpy(tmp, s, n);
    tmp[n] = '\0';
    ustrlist_add(sl, tmp);
    free(tmp);
}

#ifdef _WIN32
  #define uio__mkdir(p)   _mkdir(p)
  #define uio__unlink(p)  _unlink(p)
  #define uio__rmdir(p)   _rmdir(p)
#else
  #define uio__mkdir(p)   mkdir(p, 0755)
  #define uio__unlink(p)  unlink(p)
  #define uio__rmdir(p)   rmdir(p)
#endif

/* ------------------------------------------------------------
 * File Read
 * ------------------------------------------------------------ */

char *uio_read_len(const char *path, size_t *out_len)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    fseek(f, 0, SEEK_SET);

    char *buf = (char *)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t rd = fread(buf, 1, (size_t)sz, f);
    buf[rd] = '\0';
    fclose(f);

    if (out_len) *out_len = rd;
    return buf;
}

char *uio_read(const char *path)
{
    return uio_read_len(path, NULL);
}

unsigned char *uio_read_bytes(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    fseek(f, 0, SEEK_SET);

    unsigned char *buf = (unsigned char *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }

    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);

    if (out_size) *out_size = rd;
    return buf;
}

char *uio_read_head(const char *path, size_t n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    char *buf = (char *)malloc(n + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t rd = fread(buf, 1, n, f);
    buf[rd] = '\0';
    fclose(f);
    return buf;
}

char *uio_read_tail(const char *path, size_t n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }

    size_t to_read = ((size_t)sz < n) ? (size_t)sz : n;
    fseek(f, -(long)to_read, SEEK_END);

    char *buf = (char *)malloc(to_read + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t rd = fread(buf, 1, to_read, f);
    buf[rd] = '\0';
    fclose(f);
    return buf;
}

/* ------------------------------------------------------------
 * Line Reading
 * ------------------------------------------------------------ */

int uio_line_open(ULineReader *r, const char *path)
{
    r->file = fopen(path, "r");
    if (!r->file) return 0;
    r->buf_size = 256;
    r->buffer = (char *)malloc(r->buf_size);
    if (!r->buffer) { fclose(r->file); r->file = NULL; return 0; }
    r->line_len = 0;
    return 1;
}

char *uio_line_next(ULineReader *r)
{
    if (!r->file) return NULL;

    size_t pos = 0;

    while (1) {
        if (pos + 2 >= r->buf_size) {
            r->buf_size *= 2;
            char *nb = (char *)realloc(r->buffer, r->buf_size);
            if (!nb) return NULL;
            r->buffer = nb;
        }

        if (!fgets(r->buffer + pos, (int)(r->buf_size - pos), r->file)) {
            if (pos == 0) return NULL;
            break;
        }

        pos += strlen(r->buffer + pos);

        if (pos > 0 && r->buffer[pos - 1] == '\n') {
            r->buffer[--pos] = '\0';
            if (pos > 0 && r->buffer[pos - 1] == '\r')
                r->buffer[--pos] = '\0';
            break;
        }
    }

    r->line_len = pos;
    return r->buffer;
}

void uio_line_close(ULineReader *r)
{
    if (r->file) { fclose(r->file); r->file = NULL; }
    free(r->buffer);
    r->buffer = NULL;
    r->buf_size = 0;
    r->line_len = 0;
}

UStrList *uio_readlines(const char *path)
{
    ULineReader r;
    if (!uio_line_open(&r, path)) return NULL;

    UStrList *sl = ustrlist_new();
    char *line;
    while ((line = uio_line_next(&r)) != NULL) {
        ustrlist_add(sl, line);
    }
    uio_line_close(&r);
    return sl;
}

UStrList *uio_readlines_count(const char *path, int *out_count)
{
    UStrList *sl = uio_readlines(path);
    if (out_count) *out_count = sl ? ustrlist_size(sl) : 0;
    return sl;
}

/* ------------------------------------------------------------
 * File Write
 * ------------------------------------------------------------ */

int uio_write(const char *path, const char *content)
{
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    if (content) fputs(content, f);
    fclose(f);
    return 1;
}

int uio_writef(const char *path, const char *fmt, ...)
{
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fclose(f);
    return 1;
}

int uio_write_bytes(const char *path, const void *data, size_t size)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    size_t w = fwrite(data, 1, size, f);
    fclose(f);
    return w == size;
}

int uio_append(const char *path, const char *content)
{
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    if (content) fputs(content, f);
    fclose(f);
    return 1;
}

int uio_appendf(const char *path, const char *fmt, ...)
{
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fclose(f);
    return 1;
}

int uio_writelines(const char *path, const char **lines)
{
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    if (lines) {
        for (int i = 0; lines[i]; i++) {
            fputs(lines[i], f);
            fputc('\n', f);
        }
    }
    fclose(f);
    return 1;
}

/* ------------------------------------------------------------
 * Split / Tokenize
 * ------------------------------------------------------------ */

UStrList *uio_split(const char *str, const char *delim)
{
    if (!str || !delim) return NULL;

    UStrList *sl = ustrlist_new();
    size_t dlen = strlen(delim);

    if (dlen == 0) {
        ustrlist_add(sl, str);
        return sl;
    }

    const char *start = str;
    const char *found;

    while ((found = strstr(start, delim)) != NULL) {
        uio__strlist_add_n(sl, start, (size_t)(found - start));
        start = found + dlen;
    }

    ustrlist_add(sl, start);
    return sl;
}

UStrList *uio_splitn(const char *str, const char *delim, int max)
{
    if (!str || !delim || max < 1) return NULL;

    UStrList *sl = ustrlist_new();
    size_t dlen = strlen(delim);

    if (dlen == 0) {
        ustrlist_add(sl, str);
        return sl;
    }

    const char *start = str;
    const char *found;
    int count = 1;

    while (count < max && (found = strstr(start, delim)) != NULL) {
        uio__strlist_add_n(sl, start, (size_t)(found - start));
        start = found + dlen;
        count++;
    }

    ustrlist_add(sl, start);
    return sl;
}

UStrList *uio_words(const char *str)
{
    if (!str) return NULL;

    UStrList *sl = ustrlist_new();
    const char *p = str;

    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;

        const char *start = p;
        while (*p && !isspace((unsigned char)*p)) p++;

        uio__strlist_add_n(sl, start, (size_t)(p - start));
    }

    return sl;
}

int uio_split_inplace(char *buf, const char *delim,
                       const char **parts, int max_parts)
{
    if (!buf || !delim || !parts || max_parts < 1) return 0;

    size_t dlen = strlen(delim);
    if (dlen == 0) {
        parts[0] = buf;
        return 1;
    }

    int count = 0;
    char *start = buf;

    while (count < max_parts - 1) {
        char *found = strstr(start, delim);
        if (!found) break;
        *found = '\0';
        parts[count++] = start;
        start = found + dlen;
    }

    parts[count++] = start;
    return count;
}

char *uio_join(const UStrList *sl, const char *sep)
{
    if (!sl) return NULL;
    int n = ustrlist_size((UStrList *)sl);
    if (n == 0) {
        char *r = (char *)malloc(1);
        if (r) r[0] = '\0';
        return r;
    }

    size_t sep_len = sep ? strlen(sep) : 0;
    size_t total = 0;

    for (int i = 0; i < n; i++) {
        total += strlen(ustrlist_get((UStrList *)sl, i));
        if (i > 0) total += sep_len;
    }

    char *buf = (char *)malloc(total + 1);
    if (!buf) return NULL;

    size_t pos = 0;
    for (int i = 0; i < n; i++) {
        if (i > 0 && sep_len > 0) {
            memcpy(buf + pos, sep, sep_len);
            pos += sep_len;
        }
        const char *s = ustrlist_get((UStrList *)sl, i);
        size_t slen = strlen(s);
        memcpy(buf + pos, s, slen);
        pos += slen;
    }
    buf[pos] = '\0';
    return buf;
}

void uio_join_buf(const UStrList *sl, const char *sep,
                   char *buf, size_t size)
{
    if (!buf || size == 0) return;
    buf[0] = '\0';
    if (!sl) return;

    int n = ustrlist_size((UStrList *)sl);
    size_t sep_len = sep ? strlen(sep) : 0;
    size_t pos = 0;

    for (int i = 0; i < n && pos < size - 1; i++) {
        if (i > 0 && sep_len > 0 && pos + sep_len < size - 1) {
            memcpy(buf + pos, sep, sep_len);
            pos += sep_len;
        }
        const char *s = ustrlist_get((UStrList *)sl, i);
        size_t slen = strlen(s);
        size_t avail = size - 1 - pos;
        size_t cpy = slen < avail ? slen : avail;
        memcpy(buf + pos, s, cpy);
        pos += cpy;
    }
    buf[pos] = '\0';
}

/* ------------------------------------------------------------
 * User Input
 * ------------------------------------------------------------ */

static char uio__input_buf[4096];

static void uio__strip_newline(char *buf)
{
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') buf[--len] = '\0';
}

const char *uio_input(const char *prompt)
{
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    if (!fgets(uio__input_buf, (int)sizeof(uio__input_buf), stdin)) {
        uio__input_buf[0] = '\0';
        return uio__input_buf;
    }
    uio__strip_newline(uio__input_buf);
    return uio__input_buf;
}

void uio_input_buf(const char *prompt, char *buf, size_t size)
{
    if (!buf || size == 0) return;
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        return;
    }
    uio__strip_newline(buf);
}

int uio_input_int(const char *prompt)
{
    char line[256];
    while (1) {
        if (prompt) { printf("%s", prompt); fflush(stdout); }
        if (!fgets(line, (int)sizeof(line), stdin)) return 0;
        uio__strip_newline(line);
        char *end;
        errno = 0;
        long val = strtol(line, &end, 10);
        if (end != line && *end == '\0' && errno == 0)
            return (int)val;
    }
}

int uio_input_int_default(const char *prompt, int def)
{
    char line[256];
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    if (!fgets(line, (int)sizeof(line), stdin)) return def;
    uio__strip_newline(line);
    if (line[0] == '\0') return def;
    char *end;
    errno = 0;
    long val = strtol(line, &end, 10);
    if (end != line && *end == '\0' && errno == 0) return (int)val;
    return def;
}

int uio_input_int_range(const char *prompt, int lo, int hi)
{
    char line[256];
    while (1) {
        if (prompt) { printf("%s", prompt); fflush(stdout); }
        if (!fgets(line, (int)sizeof(line), stdin)) return lo;
        uio__strip_newline(line);
        char *end;
        errno = 0;
        long val = strtol(line, &end, 10);
        if (end != line && *end == '\0' && errno == 0 &&
            val >= lo && val <= hi)
            return (int)val;
    }
}

double uio_input_double(const char *prompt)
{
    char line[256];
    while (1) {
        if (prompt) { printf("%s", prompt); fflush(stdout); }
        if (!fgets(line, (int)sizeof(line), stdin)) return 0.0;
        uio__strip_newline(line);
        char *end;
        errno = 0;
        double val = strtod(line, &end);
        if (end != line && *end == '\0' && errno == 0)
            return val;
    }
}

double uio_input_double_default(const char *prompt, double def)
{
    char line[256];
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    if (!fgets(line, (int)sizeof(line), stdin)) return def;
    uio__strip_newline(line);
    if (line[0] == '\0') return def;
    char *end;
    errno = 0;
    double val = strtod(line, &end);
    if (end != line && *end == '\0' && errno == 0) return val;
    return def;
}

int uio_confirm(const char *prompt)
{
    char line[64];
    while (1) {
        if (prompt) printf("%s [o/n]: ", prompt);
        fflush(stdout);
        if (!fgets(line, (int)sizeof(line), stdin)) return 0;
        uio__strip_newline(line);
        if (line[0] == 'o' || line[0] == 'O' ||
            line[0] == 'y' || line[0] == 'Y' ||
            strcmp(line, "oui") == 0 || strcmp(line, "yes") == 0)
            return 1;
        if (line[0] == 'n' || line[0] == 'N' ||
            strcmp(line, "non") == 0 || strcmp(line, "no") == 0)
            return 0;
    }
}

int uio_confirm_default(const char *prompt, int def)
{
    char line[64];
    if (prompt) {
        printf("%s [%s]: ", prompt, def ? "O/n" : "o/N");
        fflush(stdout);
    }
    if (!fgets(line, (int)sizeof(line), stdin)) return def;
    uio__strip_newline(line);
    if (line[0] == '\0') return def;
    if (line[0] == 'o' || line[0] == 'O' ||
        line[0] == 'y' || line[0] == 'Y') return 1;
    if (line[0] == 'n' || line[0] == 'N') return 0;
    return def;
}

int uio_choose(const char *prompt, const char **options)
{
    if (!options) return -1;
    int n = 0;
    while (options[n]) n++;
    if (n == 0) return -1;

    char line[64];
    while (1) {
        if (prompt) printf("%s\n", prompt);
        for (int i = 0; i < n; i++)
            printf("  %d) %s\n", i + 1, options[i]);
        printf("Choix: ");
        fflush(stdout);
        if (!fgets(line, (int)sizeof(line), stdin)) return 0;
        uio__strip_newline(line);
        char *end;
        long val = strtol(line, &end, 10);
        if (end != line && *end == '\0' && val >= 1 && val <= n)
            return (int)(val - 1);
    }
}

void uio_password(const char *prompt, char *buf, size_t size)
{
    if (!buf || size == 0) return;
    if (prompt) { printf("%s", prompt); fflush(stdout); }

#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    int console = GetConsoleMode(h, &mode);
    if (console) SetConsoleMode(h, mode & ~ENABLE_ECHO_INPUT);
    if (fgets(buf, (int)size, stdin)) uio__strip_newline(buf);
    else buf[0] = '\0';
    if (console) SetConsoleMode(h, mode);
#else
    struct termios old, cur;
    int tty = tcgetattr(fileno(stdin), &old) == 0;
    if (tty) {
        cur = old;
        cur.c_lflag &= ~(tcflag_t)ECHO;
        tcsetattr(fileno(stdin), TCSANOW, &cur);
    }
    if (fgets(buf, (int)size, stdin)) uio__strip_newline(buf);
    else buf[0] = '\0';
    if (tty) tcsetattr(fileno(stdin), TCSANOW, &old);
#endif
    printf("\n");
}

/* ------------------------------------------------------------
 * Filesystem
 * ------------------------------------------------------------ */

int uio_exists(const char *path)
{
#ifdef _WIN32
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat st;
    return stat(path, &st) == 0;
#endif
}

int uio_is_file(const char *path)
{
#ifdef _WIN32
    DWORD a = GetFileAttributesA(path);
    return (a != INVALID_FILE_ATTRIBUTES) && !(a & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
#endif
}

int uio_is_dir(const char *path)
{
#ifdef _WIN32
    DWORD a = GetFileAttributesA(path);
    return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}

long long uio_filesize(const char *path)
{
#ifdef _WIN32
    struct _stat64 st;
    if (_stat64(path, &st) != 0) return -1;
    return (long long)st.st_size;
#else
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    return (long long)st.st_size;
#endif
}

time_t uio_mtime(const char *path)
{
#ifdef _WIN32
    struct _stat64 st;
    if (_stat64(path, &st) != 0) return (time_t)-1;
    return st.st_mtime;
#else
    struct stat st;
    if (stat(path, &st) != 0) return (time_t)-1;
    return st.st_mtime;
#endif
}

int uio_copy(const char *src, const char *dst)
{
    FILE *in = fopen(src, "rb");
    if (!in) return 0;
    FILE *out = fopen(dst, "wb");
    if (!out) { fclose(in); return 0; }

    char buf[8192];
    size_t n;
    int ok = 1;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) { ok = 0; break; }
    }

    fclose(in);
    fclose(out);
    return ok;
}

int uio_move(const char *src, const char *dst)
{
    if (rename(src, dst) == 0) return 1;
    if (!uio_copy(src, dst)) return 0;
    return uio_remove_file(src);
}

int uio_remove_file(const char *path)
{
    return uio__unlink(path) == 0;
}

int uio_mkdir_p(const char *path)
{
    if (uio_is_dir(path)) return 1;

    char tmp[PATH_MAX];
    snprintf(tmp, sizeof(tmp), "%s", path);

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char saved = *p;
            *p = '\0';
            if (!uio_is_dir(tmp)) {
                if (uio__mkdir(tmp) != 0 && errno != EEXIST) return 0;
            }
            *p = saved;
        }
    }

    if (uio__mkdir(tmp) != 0 && errno != EEXIST) return 0;
    return 1;
}

int uio_rmdir_empty(const char *path)
{
    return uio__rmdir(path) == 0;
}

int uio_rmdir_all(const char *path)
{
    if (uio_is_file(path)) return uio_remove_file(path);
    if (!uio_is_dir(path)) return 0;

#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char pattern[PATH_MAX];
    snprintf(pattern, sizeof(pattern), "%s\\*", path);
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    int ok = 1;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;
        char child[PATH_MAX + 256];
        snprintf(child, sizeof(child), "%s\\%s", path, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!uio_rmdir_all(child)) ok = 0;
        } else {
            if (!uio_remove_file(child)) ok = 0;
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(path);
    if (!d) return 0;
    int ok = 1;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char child[PATH_MAX];
        snprintf(child, sizeof(child), "%s/%s", path, entry->d_name);
        if (uio_is_dir(child)) {
            if (!uio_rmdir_all(child)) ok = 0;
        } else {
            if (!uio_remove_file(child)) ok = 0;
        }
    }
    closedir(d);
#endif

    if (uio__rmdir(path) != 0) ok = 0;
    return ok;
}

int uio_tempfile(char *buf, size_t size)
{
    if (!buf || size == 0) return 0;
#ifdef _WIN32
    char tmpdir[MAX_PATH];
    GetTempPathA(MAX_PATH, tmpdir);
    char tmpname[MAX_PATH];
    if (GetTempFileNameA(tmpdir, "uio", 0, tmpname) == 0) return 0;
    snprintf(buf, size, "%s", tmpname);
    return 1;
#else
    char tmpl[] = "/tmp/uio_XXXXXX";
    int fd = mkstemp(tmpl);
    if (fd < 0) return 0;
    close(fd);
    snprintf(buf, size, "%s", tmpl);
    return 1;
#endif
}

int uio_tempdir(char *buf, size_t size)
{
    if (!buf || size == 0) return 0;
#ifdef _WIN32
    char tmpdir[MAX_PATH];
    GetTempPathA(MAX_PATH, tmpdir);
    char name[MAX_PATH + 64];
    snprintf(name, sizeof(name), "%suio_dir_%lx",
             tmpdir, (unsigned long)GetTickCount());
    if (!uio_mkdir_p(name)) return 0;
    snprintf(buf, size, "%s", name);
    return 1;
#else
    char tmpl[] = "/tmp/uio_dir_XXXXXX";
    char *result = mkdtemp(tmpl);
    if (!result) return 0;
    snprintf(buf, size, "%s", result);
    return 1;
#endif
}

/* ------------------------------------------------------------
 * Directory Listing
 * ------------------------------------------------------------ */

UStrList *uio_listdir(const char *path)
{
    UStrList *sl = ustrlist_new();

#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char pattern[PATH_MAX];
    snprintf(pattern, sizeof(pattern), "%s\\*", path);
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return sl;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;
        ustrlist_add(sl, fd.cFileName);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(path);
    if (!d) return sl;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        ustrlist_add(sl, entry->d_name);
    }
    closedir(d);
#endif

    return sl;
}

UStrList *uio_listdir_ext(const char *path, const char *ext)
{
    UStrList *all = uio_listdir(path);
    if (!ext) return all;

    UStrList *filtered = ustrlist_new();
    size_t ext_len = strlen(ext);

    for (int i = 0; i < ustrlist_size(all); i++) {
        const char *name = ustrlist_get(all, i);
        size_t nlen = strlen(name);
        if (nlen >= ext_len &&
            strcmp(name + nlen - ext_len, ext) == 0) {
            ustrlist_add(filtered, name);
        }
    }

    ustrlist_free(all);
    return filtered;
}

static void uio__listdir_rec(const char *base, const char *prefix,
                              UStrList *sl)
{
#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char pattern[PATH_MAX];
    snprintf(pattern, sizeof(pattern), "%s\\*", base);
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;
        char child[PATH_MAX + 256], rel[PATH_MAX + 256];
        snprintf(child, sizeof(child), "%s\\%s", base, fd.cFileName);
        if (prefix[0])
            snprintf(rel, sizeof(rel), "%s%c%s", prefix, UIO_SEP, fd.cFileName);
        else
            snprintf(rel, sizeof(rel), "%s", fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            uio__listdir_rec(child, rel, sl);
        else
            ustrlist_add(sl, rel);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(base);
    if (!d) return;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char child[PATH_MAX], rel[PATH_MAX];
        snprintf(child, sizeof(child), "%s/%s", base, entry->d_name);
        if (prefix[0])
            snprintf(rel, sizeof(rel), "%s/%s", prefix, entry->d_name);
        else
            snprintf(rel, sizeof(rel), "%s", entry->d_name);
        if (uio_is_dir(child))
            uio__listdir_rec(child, rel, sl);
        else
            ustrlist_add(sl, rel);
    }
    closedir(d);
#endif
}

UStrList *uio_listdir_recursive(const char *path)
{
    UStrList *sl = ustrlist_new();
    uio__listdir_rec(path, "", sl);
    return sl;
}

/* ------------------------------------------------------------
 * Paths
 * ------------------------------------------------------------ */

void uio_basename(const char *path, char *buf, size_t size)
{
    if (!path || !buf || size == 0) return;
    const char *last = path;
    for (const char *p = path; *p; p++) {
        if (*p == '/' || *p == '\\') last = p + 1;
    }
    snprintf(buf, size, "%s", last);
}

void uio_dirname(const char *path, char *buf, size_t size)
{
    if (!path || !buf || size == 0) return;
    const char *last_sep = NULL;
    for (const char *p = path; *p; p++) {
        if (*p == '/' || *p == '\\') last_sep = p;
    }
    if (!last_sep) {
        snprintf(buf, size, ".");
    } else {
        size_t len = (size_t)(last_sep - path);
        if (len == 0) len = 1;
        if (len >= size) len = size - 1;
        memcpy(buf, path, len);
        buf[len] = '\0';
    }
}

void uio_extension(const char *path, char *buf, size_t size)
{
    if (!path || !buf || size == 0) return;
    const char *dot = NULL;
    const char *last_sep = NULL;
    for (const char *p = path; *p; p++) {
        if (*p == '/' || *p == '\\') last_sep = p;
        if (*p == '.') dot = p;
    }
    if (!dot || (last_sep && dot < last_sep) || dot == path) {
        buf[0] = '\0';
        return;
    }
    snprintf(buf, size, "%s", dot + 1);
}

void uio_join_path(const char *a, const char *b, char *buf, size_t size)
{
    if (!a || !b || !buf || size == 0) return;
    size_t alen = strlen(a);
    int a_has_sep = alen > 0 && (a[alen - 1] == '/' || a[alen - 1] == '\\');
    int b_has_sep = b[0] == '/' || b[0] == '\\';

    if (a_has_sep && b_has_sep)
        snprintf(buf, size, "%s%s", a, b + 1);
    else if (a_has_sep || b_has_sep)
        snprintf(buf, size, "%s%s", a, b);
    else
        snprintf(buf, size, "%s%c%s", a, UIO_SEP, b);
}

void uio_join_path3(const char *a, const char *b, const char *c,
                     char *buf, size_t size)
{
    char tmp[PATH_MAX];
    uio_join_path(a, b, tmp, sizeof(tmp));
    uio_join_path(tmp, c, buf, size);
}

void uio_realpath_buf(const char *path, char *buf, size_t size)
{
    if (!path || !buf || size == 0) return;
#ifdef _WIN32
    if (!_fullpath(buf, path, size)) buf[0] = '\0';
#else
    char *resolved = realpath(path, NULL);
    if (resolved) {
        snprintf(buf, size, "%s", resolved);
        free(resolved);
    } else {
        buf[0] = '\0';
    }
#endif
}

/* ------------------------------------------------------------
 * CSV
 * ------------------------------------------------------------ */

static int uio__csv_init(UCSV *csv, const char *path, char delimiter,
                          int read_header)
{
    memset(csv, 0, sizeof(*csv));
    csv->file = fopen(path, "r");
    if (!csv->file) return 0;

    csv->delimiter = delimiter;
    csv->line_buf_size = 1024;
    csv->line_buf = (char *)malloc(csv->line_buf_size);
    csv->field_buf_size = 1024;
    csv->field_buf = (char *)malloc(csv->field_buf_size);
    csv->field_cap = 32;
    csv->fields = (char **)malloc((size_t)csv->field_cap * sizeof(char *));

    if (!csv->line_buf || !csv->field_buf || !csv->fields) {
        uio_csv_close(csv);
        return 0;
    }

    csv->has_header = read_header;
    if (read_header) {
        char **row = uio_csv_next(csv);
        if (row) {
            csv->header_count = csv->col_count;
            csv->headers = (char **)malloc(
                (size_t)csv->header_count * sizeof(char *));
            for (int i = 0; i < csv->header_count; i++)
                csv->headers[i] = strdup(row[i]);
            csv->row_number = 0;
        }
    }

    return 1;
}

int uio_csv_open(UCSV *csv, const char *path, char delimiter)
{
    return uio__csv_init(csv, path, delimiter, 0);
}

int uio_csv_open_header(UCSV *csv, const char *path, char delimiter)
{
    return uio__csv_init(csv, path, delimiter, 1);
}

static void uio__csv_grow_field_buf(UCSV *csv, size_t need)
{
    if (need < csv->field_buf_size) return;
    while (csv->field_buf_size <= need) csv->field_buf_size *= 2;
    csv->field_buf = (char *)realloc(csv->field_buf, csv->field_buf_size);
}

static void uio__csv_add_field(UCSV *csv, const char *data, size_t len)
{
    if (csv->col_count >= csv->field_cap) {
        csv->field_cap *= 2;
        csv->fields = (char **)realloc(csv->fields,
            (size_t)csv->field_cap * sizeof(char *));
    }
    /* Store the field: copy into field_buf is already done by caller */
    (void)len;
    csv->fields[csv->col_count++] = (char *)data;
}

char **uio_csv_next(UCSV *csv)
{
    if (!csv->file) return NULL;

    csv->col_count = 0;
    size_t fpos = 0;   /* position in field_buf */
    size_t fstart = 0; /* start of current field in field_buf */
    int in_quotes = 0;
    int field_started = 0;
    int got_data = 0;

    while (1) {
        int c = fgetc(csv->file);

        if (c == EOF) {
            if (!got_data && !field_started) return NULL;
            /* end last field */
            uio__csv_grow_field_buf(csv, fpos + 1);
            csv->field_buf[fpos] = '\0';
            uio__csv_add_field(csv, csv->field_buf + fstart, fpos - fstart);
            csv->row_number++;
            return csv->fields;
        }

        got_data = 1;
        field_started = 1;

        if (in_quotes) {
            if (c == '"') {
                int next = fgetc(csv->file);
                if (next == '"') {
                    /* escaped quote */
                    uio__csv_grow_field_buf(csv, fpos + 1);
                    csv->field_buf[fpos++] = '"';
                } else {
                    in_quotes = 0;
                    if (next != EOF) ungetc(next, csv->file);
                }
            } else {
                uio__csv_grow_field_buf(csv, fpos + 1);
                csv->field_buf[fpos++] = (char)c;
            }
        } else {
            if (c == '"' && fpos == fstart) {
                in_quotes = 1;
            } else if (c == csv->delimiter) {
                uio__csv_grow_field_buf(csv, fpos + 1);
                csv->field_buf[fpos] = '\0';
                uio__csv_add_field(csv, csv->field_buf + fstart, fpos - fstart);
                fpos++;
                fstart = fpos;
                field_started = 0;
            } else if (c == '\n') {
                if (fpos > fstart && csv->field_buf[fpos - 1] == '\r') fpos--;
                uio__csv_grow_field_buf(csv, fpos + 1);
                csv->field_buf[fpos] = '\0';
                uio__csv_add_field(csv, csv->field_buf + fstart, fpos - fstart);
                csv->row_number++;
                return csv->fields;
            } else {
                uio__csv_grow_field_buf(csv, fpos + 1);
                csv->field_buf[fpos++] = (char)c;
            }
        }
    }
}

const char *uio_csv_get(const UCSV *csv, char **row, const char *col)
{
    if (!csv->has_header || !csv->headers || !row) return NULL;
    for (int i = 0; i < csv->header_count; i++) {
        if (strcmp(csv->headers[i], col) == 0) {
            if (i < csv->col_count) return row[i];
            return NULL;
        }
    }
    return NULL;
}

void uio_csv_close(UCSV *csv)
{
    if (csv->file) { fclose(csv->file); csv->file = NULL; }
    free(csv->line_buf);   csv->line_buf = NULL;
    free(csv->field_buf);  csv->field_buf = NULL;
    free(csv->fields);     csv->fields = NULL;
    if (csv->headers) {
        for (int i = 0; i < csv->header_count; i++) free(csv->headers[i]);
        free(csv->headers);
        csv->headers = NULL;
    }
}

int uio_csv_writer_open(UCSVWriter *w, const char *path, char delimiter)
{
    w->file = fopen(path, "w");
    if (!w->file) return 0;
    w->delimiter = delimiter;
    return 1;
}

void uio_csv_write_row(UCSVWriter *w, const char **fields)
{
    if (!w->file || !fields) return;

    for (int i = 0; fields[i]; i++) {
        if (i > 0) fputc(w->delimiter, w->file);

        int needs_quote = 0;
        for (const char *p = fields[i]; *p; p++) {
            if (*p == w->delimiter || *p == '"' || *p == '\n' || *p == '\r') {
                needs_quote = 1;
                break;
            }
        }

        if (needs_quote) {
            fputc('"', w->file);
            for (const char *p = fields[i]; *p; p++) {
                if (*p == '"') fputc('"', w->file);
                fputc(*p, w->file);
            }
            fputc('"', w->file);
        } else {
            fputs(fields[i], w->file);
        }
    }
    fputc('\n', w->file);
}

void uio_csv_writer_close(UCSVWriter *w)
{
    if (w->file) { fclose(w->file); w->file = NULL; }
}

#endif /* UIO_IMPLEMENTATION */

#endif /* UIO_H */
