SRCS = allocator/allocator.c common/bump.c

allocator: $(SRCS)
	gcc -Wall -Wextra -g -o build/allocator $(SRCS)

unit: tests/unit_tests.c $(SRCS)
	gcc -Wall -Wextra -g -o build/unit_tests $(SRCS) tests/unit_tests.c

bench: tests/benchmark.c $(SRCS)
	gcc -Wall -Wextra -g -o build/benchmark $(SRCS) tests/benchmark.c
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
