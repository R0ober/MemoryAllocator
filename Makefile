my_malloc: malloc.c
	gcc -Wall -Wextra -g -o malloc malloc.c


test: test.c malloc.c
	gcc -Wall -Wextra -g -o test malloc.c test.c

unit: tests/unit_tests.c malloc.c
	gcc -Wall -Wextra -g -o unit_tests malloc.c tests/unit_tests.c

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
