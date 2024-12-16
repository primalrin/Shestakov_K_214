#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>

typedef enum
{
    IS_FINITE_REP_OK = 0,
    IS_FINITE_REP_INVALID_BASE,
    IS_FINITE_REP_OVERFLOW,
    IS_FINITE_REP_MAX_ITER_REACHED
} IsFiniteRepStatus;

typedef enum
{
    CHECK_NUMBERS_OK = 0,
    CHECK_NUMBERS_INVALID_BASE,
    CHECK_NUMBERS_INVALID_COUNT,
    CHECK_NUMBERS_BASE_TOO_LARGE,
    CHECK_NUMBERS_INVALID_NUMBER_RANGE,
    CHECK_NUMBERS_IS_FINITE_REP_ERROR
} CheckNumbersStatus;

IsFiniteRepStatus is_finite_representation(double number, int base, bool *result)
{
    if (base < 2)
    {
        return IS_FINITE_REP_INVALID_BASE;
    }
    if (!result)
    {
        return IS_FINITE_REP_INVALID_BASE;
    }
    double fractional_part = number;
    const int MAX_ITERATIONS = 1000;
    int iterations = 0;

    while (fractional_part > 1e-15 && iterations < MAX_ITERATIONS)
    {
        if (fabs(fractional_part) > DBL_MAX / base)
        {
            return IS_FINITE_REP_OVERFLOW;
        }
        fractional_part *= base;
        int integer_part = (int)fractional_part;
        fractional_part -= integer_part;
        iterations++;
    }

    if (iterations >= MAX_ITERATIONS && fractional_part > 1e-15)
    {
        *result = false;
        return IS_FINITE_REP_MAX_ITER_REACHED;
    }

    *result = fractional_part < 1e-15;
    return IS_FINITE_REP_OK;
}

CheckNumbersStatus check_numbers(int base, int count, ...)
{
    if (base < 2)
    {
        printf("Error: Base must be greater than or equal to 2.\n");
        return CHECK_NUMBERS_INVALID_BASE;
    }
    if (count <= 0)
    {
        printf("Error: Invalid count of numbers.\n");
        return CHECK_NUMBERS_INVALID_COUNT;
    }

    if (base > INT_MAX / 2)
    {
        printf("Error: Base is too large to prevent potential overflow.\n");
        return CHECK_NUMBERS_BASE_TOO_LARGE;
    }

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i)
    {
        double number = va_arg(args, double);

        if (number <= 0 || number >= 1)
        {
            printf("Error: Numbers must be in the range (0, 1).\n");
            va_end(args);
            return CHECK_NUMBERS_INVALID_NUMBER_RANGE;
        }
        bool finite;
        IsFiniteRepStatus is_finite_status = is_finite_representation(number, base, &finite);
        if (is_finite_status != IS_FINITE_REP_OK && is_finite_status != IS_FINITE_REP_MAX_ITER_REACHED)
        {
            printf("Error while checking if number is finite.\n");
            if (is_finite_status == IS_FINITE_REP_INVALID_BASE)
            {
                printf("Invalid base passed to is_finite_representation.\n");
            }
            else if (is_finite_status == IS_FINITE_REP_OVERFLOW)
            {
                printf("Potential overflow detected in is_finite_representation.\n");
            }
            va_end(args);
            return CHECK_NUMBERS_IS_FINITE_REP_ERROR;
        }

        if (finite)
        {
            printf("Number %.6f has a finite representation in base %d.\n", number, base);
        }
        else
        {
            printf("Number %.6f does not have a finite representation in base %d.\n", number, base);
        }
    }

    va_end(args);
    return CHECK_NUMBERS_OK;
}

int main(void)
{

    printf("Test 1: Binary system (base 2)\n");
    if (check_numbers(2, 3, 0.5, 0.25, 0.125) != CHECK_NUMBERS_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\nTest 2: Decimal system (base 10)\n");
    if (check_numbers(10, 2, 0.1, 0.3333) != CHECK_NUMBERS_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\nTest 3: Ternary system (base 3)\n");
    if (check_numbers(3, 2, 1.0 / 3.0, 0.2) != CHECK_NUMBERS_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\nTest 4: Invalid base\n");
    if (check_numbers(1, 2, 0.5, 0.25) != CHECK_NUMBERS_INVALID_BASE)
    {
        printf("Error: Test 4 failed. Expected CHECK_NUMBERS_INVALID_BASE.\n");
        return EXIT_FAILURE;
    }

    printf("\nTest 5: Invalid count\n");
    if (check_numbers(2, 0, 0.5, 0.25) != CHECK_NUMBERS_INVALID_COUNT)
    {
        printf("Error: Test 5 failed. Expected CHECK_NUMBERS_INVALID_COUNT.\n");
        return EXIT_FAILURE;
    }

    printf("\nTest 6: Invalid number range\n");
    if (check_numbers(2, 2, 1.5, 0.25) != CHECK_NUMBERS_INVALID_NUMBER_RANGE)
    {
        printf("Error: Test 6 failed. Expected CHECK_NUMBERS_INVALID_NUMBER_RANGE.\n");
        return EXIT_FAILURE;
    }

    printf("\nTest 7: Large base\n");
    if (check_numbers(INT_MAX / 2 + 1, 2, 0.5, 0.25) != CHECK_NUMBERS_BASE_TOO_LARGE)
    {
        printf("Error: Test 7 failed. Expected CHECK_NUMBERS_BASE_TOO_LARGE.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}