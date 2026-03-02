# UIO

Universal I/O for C — file read/write, split, user input, filesystem, CSV — all in one header.

Depends on [UList](https://github.com/jojo8356/ulist) for `UStrList`.

## Quick Start

### Header-only (stb-style)

```c
#define UIO_IMPLEMENTATION
#include "uio.h"    // in exactly ONE .c file
```

```c
#include "uio.h"    // declarations only, in other files
```

### As a static library

```bash
git clone https://github.com/jojo8356/ulist.git
git clone https://github.com/jojo8356/uio.git
cd uio
make build    # builds libuio.a (requires ../ulist/)
```

```bash
gcc -I. -I../ulist/include main.c -Lbuild -luio -L../ulist/build -lulist -o main
```

## Usage

### Read a file

```c
// Read entire file (malloc'd — free() when done)
char *content = uio_read("config.txt");
printf("%s", content);
free(content);

// With length
size_t len;
char *data = uio_read_len("config.txt", &len);

// Binary
size_t size;
unsigned char *bin = uio_read_bytes("image.png", &size);

// Head / Tail
char *head = uio_read_head("big.log", 1024);
char *tail = uio_read_tail("big.log", 1024);
```

### Read lines

```c
// All lines at once (returns UStrList*)
UStrList *lines = uio_readlines("data.txt");
for (int i = 0; i < ustrlist_size(lines); i++)
    printf("%s\n", ustrlist_get(lines, i));
ustrlist_free(lines);

// Streaming (constant memory)
ULineReader r;
if (uio_line_open(&r, "huge.log")) {
    char *line;
    while ((line = uio_line_next(&r)) != NULL)
        printf("%s\n", line);
    uio_line_close(&r);
}
```

### Write a file

```c
uio_write("output.txt", "Hello, World!\n");
uio_writef("output.txt", "Score: %d/%d\n", 42, 50);
uio_append("log.txt", "New entry\n");
uio_appendf("log.txt", "[%s] %s\n", ts, msg);
uio_write_bytes("output.bin", data, size);

const char *lines[] = {"line 1", "line 2", "line 3", NULL};
uio_writelines("output.txt", lines);
```

### Split / Tokenize (replaces strtok)

```c
// Split — preserves empty fields, thread-safe
UStrList *parts = uio_split("a,,b,c", ",");
// parts: ["a", "", "b", "c"]  — empty field preserved!
ustrlist_free(parts);

// Split with limit
UStrList *parts = uio_splitn("a,b,c,d,e", ",", 3);
// parts: ["a", "b", "c,d,e"]

// Split on whitespace
UStrList *words = uio_words("  hello   world  ");
// words: ["hello", "world"]

// Zero-alloc split (modifies buffer)
char buf[] = "192.168.1.100";
const char *octets[4];
int n = uio_split_inplace(buf, ".", octets, 4);

// Join
char *joined = uio_join(parts, ", ");  // "a, b, c"
free(joined);
```

### User Input

```c
// Read a string (static buffer — valid until next call)
const char *name = uio_input("Name: ");

// Thread-safe version
char name[64];
uio_input_buf("Name: ", name, sizeof(name));

// Read int (retries on invalid input)
int age = uio_input_int("Age: ");
int port = uio_input_int_default("Port [8080]: ", 8080);
int note = uio_input_int_range("Note (0-20): ", 0, 20);

// Read double
double price = uio_input_double("Price: ");

// Yes/No
int ok = uio_confirm("Delete file?");        // [o/n]
int ok = uio_confirm_default("Continue?", 1); // [O/n]

// Menu
const char *opts[] = {"Easy", "Normal", "Hard", NULL};
int choice = uio_choose("Difficulty:", opts);

// Password (hidden input)
char pass[128];
uio_password("Password: ", pass, sizeof(pass));
```

### Filesystem

```c
uio_exists("config.txt");       // 1 if exists
uio_is_file("config.txt");      // 1 if regular file
uio_is_dir("src/");             // 1 if directory
uio_filesize("data.bin");       // size in bytes (-1 on error)
uio_mtime("config.txt");        // modification time

uio_copy("src.txt", "dst.txt");
uio_move("old.txt", "new.txt");
uio_remove_file("temp.txt");
uio_mkdir_p("path/to/dir");     // mkdir -p
uio_rmdir_all("path/to/dir");   // rm -rf

// Temp files
char path[PATH_MAX];
uio_tempfile(path, sizeof(path));
uio_tempdir(path, sizeof(path));
```

### Directory Listing

```c
UStrList *all = uio_listdir("src/");
UStrList *c_files = uio_listdir_ext("src/", ".c");
UStrList *recursive = uio_listdir_recursive("src/");
// Free with ustrlist_free()
```

### Paths

```c
char buf[PATH_MAX];
uio_basename("/home/user/file.txt", buf, sizeof(buf));  // "file.txt"
uio_dirname("/home/user/file.txt", buf, sizeof(buf));   // "/home/user"
uio_extension("file.tar.gz", buf, sizeof(buf));          // "gz"
uio_join_path("/home", "file.txt", buf, sizeof(buf));    // "/home/file.txt"
uio_join_path3("/home", "user", "f.txt", buf, sizeof(buf));
uio_realpath_buf("../file.txt", buf, sizeof(buf));
```

### CSV

```c
// Read
UCSV csv;
if (uio_csv_open_header(&csv, "data.csv", ',')) {
    char **row;
    while ((row = uio_csv_next(&csv)) != NULL) {
        const char *name = uio_csv_get(&csv, row, "name");
        printf("%s\n", name);
    }
    uio_csv_close(&csv);
}

// Write
UCSVWriter w;
if (uio_csv_writer_open(&w, "output.csv", ',')) {
    const char *h[] = {"name", "age", NULL};
    uio_csv_write_row(&w, h);
    const char *r[] = {"Alice", "30", NULL};
    uio_csv_write_row(&w, r);
    uio_csv_writer_close(&w);
}
```

## Comparison with standard C

### Read a file
```c
// Standard C — 12 lines
FILE *f = fopen("data.txt", "r");
fseek(f, 0, SEEK_END);
long size = ftell(f);
fseek(f, 0, SEEK_SET);
char *buf = malloc(size + 1);
fread(buf, 1, size, f);
buf[size] = '\0';
fclose(f);

// UIO — 1 line
char *buf = uio_read("data.txt");
```

### Split
```c
// strtok — not thread-safe, loses empty fields
char line[] = "a,,b";
strtok(line, ","); // "a", "b" — empty field lost!

// UIO — thread-safe, preserves empty fields
UStrList *p = uio_split("a,,b", ","); // "a", "", "b"
```

### User input
```c
// Standard C — 5 lines
char name[64];
printf("Name: ");
fflush(stdout);
fgets(name, sizeof(name), stdin);
name[strcspn(name, "\n")] = '\0';

// UIO — 1 line
const char *name = uio_input("Name: ");
```

## Memory Convention

| Function | Allocation | Free with |
|----------|-----------|-----------|
| `uio_read()` | `malloc` | `free()` |
| `uio_readlines()` | UStrList | `ustrlist_free()` |
| `uio_split()` | UStrList | `ustrlist_free()` |
| `uio_join()` | `malloc` | `free()` |
| `uio_listdir()` | UStrList | `ustrlist_free()` |
| `uio_input()` | static buffer | nothing |
| `uio_input_buf()` | user buffer | nothing |
| `uio_split_inplace()` | none | nothing |
| `uio_join_buf()` | user buffer | nothing |
| `uio_line_next()` | internal buffer | `uio_line_close()` |

## Build Targets

```bash
make build          # build libuio.a (release, -O2)
make test           # run all 85 tests
make test-asan      # tests with AddressSanitizer + UBSan
make test-valgrind  # tests with Valgrind (leak check)
make analyze        # Clang Static Analyzer
make check          # all of the above
make clean          # remove build/
```

## Quality

- **85 tests** — read, write, lines, split, input, filesystem, CSV, paths, edge cases
- **0 memory leaks** — Valgrind clean
- **0 errors** — AddressSanitizer + UndefinedBehaviorSanitizer
- **0 bugs** — Clang Static Analyzer
- Compiled with `-Wall -Wextra -Wpedantic -Werror`

## Project Structure

```
uio/
├── uio.h              # main header (declarations + implementation)
├── uio.c              # compilation unit
├── Makefile
├── tests/
│   ├── utest.h        # mini test framework
│   ├── test_main.c    # test runner (85 tests)
│   ├── test_read.c    # file read tests
│   ├── test_write.c   # file write tests
│   ├── test_lines.c   # line reading tests
│   ├── test_split.c   # split/join tests
│   ├── test_input.c   # user input tests
│   ├── test_fs.c      # filesystem tests
│   ├── test_csv.c     # CSV tests
│   ├── test_path.c    # path tests
│   └── test_edge.c    # edge case tests
└── README.md
```

## License

MIT
