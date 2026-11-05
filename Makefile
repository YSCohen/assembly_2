CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -O0 -g -fno-omit-frame-pointer
LDFLAGS := -no-pie
TARGET := test_hw1011
ALLOC_TARGET := test_alloc_fail

.PHONY: all test check-source test-alloc test-p1 test-p2 test-p3 test-p4 test-p5 test-p6 valgrind clean

all: $(TARGET) $(ALLOC_TARGET)

$(TARGET): hw1011.s tests/test_hw1011.c hw1011.h
	$(CC) $(CFLAGS) $(LDFLAGS) -I. hw1011.s tests/test_hw1011.c -o $@

$(ALLOC_TARGET): hw1011.s tests/test_alloc_fail.c tests/alloc_wrap.c hw1011.h
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--wrap=malloc -Wl,--wrap=free \
		-I. hw1011.s tests/test_alloc_fail.c tests/alloc_wrap.c -o $@

check-source:
	python3 tests/check_source.py

test: check-source $(TARGET) $(ALLOC_TARGET)
	./$(TARGET)
	./$(ALLOC_TARGET)

test-alloc: $(ALLOC_TARGET)
	./$(ALLOC_TARGET)

test-p1: $(TARGET)
	./$(TARGET) p1

test-p2: $(TARGET)
	./$(TARGET) p2

test-p3: $(TARGET)
	./$(TARGET) p3

test-p4: $(TARGET)
	./$(TARGET) p4

test-p5: $(TARGET)
	./$(TARGET) p5

test-p6: $(TARGET)
	./$(TARGET) p6

valgrind: $(TARGET)
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET)

clean:
	rm -f $(TARGET) $(ALLOC_TARGET) *.o tests/*.o
