# ============================================================
# UIO — Makefile
# ============================================================

ULIST_DIR ?= ../ulist

CC       = gcc
AR       = ar

# Directories
BUILDDIR = build
OBJDIR   = $(BUILDDIR)/obj
TESTDIR  = tests

# Sources
SRCS     = uio.c
OBJS     = $(OBJDIR)/uio.o
LIB      = $(BUILDDIR)/libuio.a

# UList
ULIST_INC     = $(ULIST_DIR)/include
ULIST_LIB_DIR = $(ULIST_DIR)/build
ULIST_LIB     = $(ULIST_LIB_DIR)/libulist.a
ULIST_SRCS    = $(wildcard $(ULIST_DIR)/src/*.c)

# Test (single compilation unit)
TEST_SRCS = $(TESTDIR)/test_main.c
TEST_BIN  = $(BUILDDIR)/test_runner

# Flags
CFLAGS_BASE  = -std=c11 -Wall -Wextra -Wpedantic -Werror -I. -I$(ULIST_INC)
CFLAGS_REL   = $(CFLAGS_BASE) -O2
CFLAGS_DBG   = $(CFLAGS_BASE) -g -O0
CFLAGS_ASAN  = $(CFLAGS_DBG) -fsanitize=address,undefined -fno-omit-frame-pointer
LDFLAGS_ASAN = -fsanitize=address,undefined

# ============================================================
# Targets
# ============================================================

.PHONY: build debug test test-asan test-valgrind analyze clean check deps

# --- Build UList dependency ---
deps:
	$(MAKE) -C $(ULIST_DIR) build CC=$(CC)

# --- Build (release) ---
build: CFLAGS = $(CFLAGS_REL)
build: $(LIB)

# --- Debug ---
debug: CFLAGS = $(CFLAGS_DBG)
debug: $(LIB)

# --- Library ---
$(LIB): $(OBJS)
	@mkdir -p $(BUILDDIR)
	$(AR) rcs $@ $^

$(OBJDIR)/uio.o: uio.c uio.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Test ---
test: CFLAGS = $(CFLAGS_DBG)
test: deps $(LIB)
	$(CC) $(CFLAGS) $(TEST_SRCS) -L$(BUILDDIR) -luio -L$(ULIST_LIB_DIR) -lulist -o $(TEST_BIN)
	@echo ""
	@echo "=== Running tests ==="
	@$(TEST_BIN)

# --- Test with ASan + UBSan ---
test-asan:
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS_ASAN) $(SRCS) $(ULIST_SRCS) $(TEST_SRCS) $(LDFLAGS_ASAN) -o $(BUILDDIR)/test_asan
	@echo ""
	@echo "=== Running tests (ASan + UBSan) ==="
	@$(BUILDDIR)/test_asan

# --- Test with Valgrind ---
test-valgrind: CFLAGS = $(CFLAGS_DBG)
test-valgrind: deps $(LIB)
	$(CC) $(CFLAGS) $(TEST_SRCS) -L$(BUILDDIR) -luio -L$(ULIST_LIB_DIR) -lulist -o $(TEST_BIN)
	@echo ""
	@echo "=== Running tests (Valgrind) ==="
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 --quiet $(TEST_BIN)

# --- Static analysis ---
analyze:
	@echo "=== Clang Static Analyzer ==="
	scan-build --status-bugs $(CC) $(CFLAGS_DBG) $(SRCS) $(ULIST_SRCS) $(TEST_SRCS) -o /dev/null

# --- Clean ---
clean:
	rm -rf $(BUILDDIR)

# --- Full check ---
check: test-asan test-valgrind analyze
	@echo ""
	@echo "========================================"
	@echo "  ALL CHECKS PASSED"
	@echo "========================================"
