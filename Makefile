allocator: allocator.c
	gcc -Wall -Wextra -g -o build/allocator allocator.c


test: test.c allocator.c
	gcc -Wall -Wextra -g -o build/test allocator.c test.c

unit: tests/unit_tests.c allocator.c
	gcc -Wall -Wextra -g -o unit_tests allocator.c tests/unit_tests.c

bench: tests/benchmark.c allocator.c
	gcc -Wall -Wextra -g -o benchmark allocator.c tests/benchmark.c
# ===============================================================
# insperation 
# CC = gcc
# CFLAGS = -Wall -Wextra -g
# TARGET = my_malloc
#
# all: $(TARGET)
#
# $(TARGET): malloc.c more.c
# 	$(CC) $(CFLAGS) -o $(TARGET) malloc.c more.c
#
# clean:
# 	rm -f $(TARGET)
# ===============================================================
