#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "../allocator/allocator.h"

extern block_header_t* free_list;
/*
    might need to add  setup() / teardown() functions to reset global state 
*/

#define RESET     "\033[0m"
#define BOLD      "\033[1m"

#define COLOR_PASS  "\033[38;5;114m"    // soft green 
#define COLOR_FAIL   "\033[38;5;203m"    // soft coral
#define COLOR_RUN "\033[38;5;248m"  // light grey 
static int SU_tests_run = 0;
static int SU_tests_failed = 0;

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        printf("%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #cond); \
        SU_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ(expected, actual) do { \
    long long _e = (long long)(expected); \
    long long _a = (long long)(actual); \
    if (_e != _a) { \
        printf("%s:%d: Assertion failed: %s == %s (expected %lld, got %lld)\n", __FILE__, __LINE__, #expected, #actual, _e, _a); \
        SU_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NOT_EQ(expected, actual) do { \
    long long _e = (long long)(expected); \
    long long _a = (long long)(actual); \
    if (_e == _a) { \
        printf("%s:%d: Assertion failed: %s != %s (expected %lld, got %lld)\n", __FILE__, __LINE__, #expected, #actual, _e, _a); \
        SU_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_PTR_NOT_NULL(p) do { \
    if ((p) == NULL) { \
        printf("%s:%d: Assertion failed: pointer is NULL: %s\n", __FILE__, __LINE__, #p); \
        SU_tests_failed++; \
        return; \
    } \
} while(0)

#define RUN_TEST(test_fn) do { \
    int _failed_before = SU_tests_failed; \
    SU_tests_run++; \
    printf(BOLD COLOR_RUN "RUNNING: %s\n" RESET, #test_fn); \
    test_fn(); \
    if (SU_tests_failed == _failed_before) { \
        printf(BOLD COLOR_PASS "  PASS: %s\n" RESET, #test_fn ); \
    } else { \
        printf(BOLD COLOR_FAIL"  FAIL: %s\n" RESET, #test_fn); \
    } \
} while(0)

static int SU_report_and_return(void) {
    printf("\nTests run: %d, Failures: %d\n", SU_tests_run, SU_tests_failed);
    return (SU_tests_failed == 0) ? 0 : 1;
}


