allocator: allocator.c
	gcc -Wall -Wextra -g -o build/allocator allocator.c

unit: tests/unit_tests.c allocator.c
	gcc -Wall -Wextra -g -o build/unit_tests allocator.c tests/unit_tests.c

bench: tests/benchmark.c allocator.c
	gcc -Wall -Wextra -g -o build/benchmark allocator.c tests/benchmark.c
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
