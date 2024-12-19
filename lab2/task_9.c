#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    SUCCESS,
    ERROR_NAN,
    ERROR_OVERFLOW,
    ERROR_INVALID_ARG,
    ERROR_MEMORY_ALLOC,
    ERROR_FILE_OPEN,
    ERROR_ITERATION_LIMIT,
    ERROR_NO_SOLUTION,
} ReturnCode;

static void log_error(ReturnCode code)
{
    switch (code)
    {
    case ERROR_NAN:
        fprintf(stderr, "Error: Found not a number\n");
        break;

    case ERROR_OVERFLOW:
        fprintf(stderr, "Error: Overflow detected\n");
        break;

    case ERROR_INVALID_ARG:
        fprintf(stderr, "Error: Invalid arguments\n");
        break;

    case ERROR_MEMORY_ALLOC:
        fprintf(stderr, "Error: Failed to allocate memory\n");
        break;

    case ERROR_FILE_OPEN:
        fprintf(stderr, "Error: Failed to open file\n");
        break;

    case ERROR_ITERATION_LIMIT:
        fprintf(stderr, "Error: Too many iterations\n");
        break;

    case ERROR_NO_SOLUTION:
        fprintf(stderr, "Error: There is no solution in your interval\n");
        break;

    default:
        fprintf(stderr, "Error: UNKNOWN ERROR CODE\n");
        break;
    }
}

static int gcd(int a, int b)
{
    while (b != 0)
    {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

static ReturnCode sieve_of_eratosthenes(bool *primes, int n)
{
    if (!primes)
    {
        return ERROR_INVALID_ARG;
    }

    for (int i = 0; i <= n; i++)
    {
        primes[i] = true;
    }
    primes[0] = primes[1] = false;
    for (int p = 2; p * p <= n; p++)
    {
        if (primes[p])
        {
            for (int i = p * p; i <= n; i += p)
            {
                primes[i] = false;
            }
        }
    }
    return SUCCESS;
}

static bool check_fraction(int numerator, int denominator, int base)
{
    bool *is_prime = (bool *)malloc(sizeof(bool) * (denominator + 1));
    if (!is_prime)
    {
        return false;
    }

    ReturnCode res = sieve_of_eratosthenes(is_prime, denominator);
    if (res != SUCCESS)
    {
        free(is_prime);
        return false;
    }

    for (int i = 2; i <= denominator; i++)
    {
        if (denominator % i == 0 && is_prime[i])
        {
            if (base % i != 0)
            {
                free(is_prime);
                return false;
            }
        }
    }
    free(is_prime);
    return true;
}

static ReturnCode double_to_fraction(double value, int *numerator, int *denominator, double tolerance)
{
    if (!numerator || !denominator)
    {
        return ERROR_INVALID_ARG;
    }

    double fractional_part = value;
    int previous_numerator = 0, current_numerator = 1;
    int previous_denominator = 1, current_denominator = 0;
    double quotient;

    while (fabs(value - (double)current_numerator / (double)current_denominator) > tolerance)
    {
        quotient = floor(fractional_part);
        int temp_numerator = current_numerator;
        int temp_denominator = current_denominator;

        current_numerator = (int)(quotient * current_numerator + previous_numerator);
        current_denominator = (int)(quotient * current_denominator + previous_denominator);

        if (current_numerator > INT_MAX || current_denominator > INT_MAX)
        {
            return ERROR_OVERFLOW;
        }

        previous_numerator = temp_numerator;
        previous_denominator = temp_denominator;

        if (fractional_part == quotient)
        {
            break;
        }

        fractional_part = 1.0 / (fractional_part - quotient);
    }

    *numerator = current_numerator;
    *denominator = current_denominator;

    int common_divisor = gcd(*numerator, *denominator);
    *numerator /= common_divisor;
    *denominator /= common_divisor;

    return SUCCESS;
}

static bool *check_fractions(ReturnCode *code, int base, int fractions_num, ...)
{
    if (base < 2 || base > 36)
    {
        *code = ERROR_INVALID_ARG;
        return NULL;
    }

    bool *result = malloc(sizeof(bool) * fractions_num);
    if (!result)
    {
        *code = ERROR_MEMORY_ALLOC;
        return NULL;
    }

    double eps = 0.000001;

    va_list args;
    va_start(args, fractions_num);
    for (int i = 0; i < fractions_num; i++)
    {
        double fraction = fabs(va_arg(args, double));
        if (fraction > 1.0 + eps)
        {
            *code = ERROR_INVALID_ARG;
            free(result);
            va_end(args);
            return NULL;
        }
        int numerator, denominator;
        ReturnCode res = double_to_fraction(fraction, &numerator, &denominator, eps);
        if (res != SUCCESS)
        {
            *code = res;
            free(result);
            va_end(args);
            return NULL;
        }

        result[i] = check_fraction(numerator, denominator, base);
    }
    va_end(args);

    return result;
}

int main(void)
{
    ReturnCode code = SUCCESS;
    int n = 3;
    double eps = 0.000001;

    bool *result = check_fractions(&code, 14, n, 0.3, 14, 15);
    if (code != SUCCESS)
    {
        log_error(code);
        return code;
    }

    for (int i = 0; i < n; i++)
    {
        printf("%d Number have a finite representation: %s\n", i + 1, result[i] ? "true" : "false");
    }

    free(result);
    return 0;
}