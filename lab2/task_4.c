#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>

int is_convex(int n, ...)
{

    if (n < 3)
    {
        fprintf(stderr, "Error: Less than three vertices provided.\n");
        return -1;
    }

    va_list args;
    va_start(args, n);

    double *x_coords = (double *)malloc((size_t)n * sizeof(double));
    double *y_coords = (double *)malloc((size_t)n * sizeof(double));

    if (x_coords == NULL || y_coords == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        if (x_coords)
            free(x_coords);
        if (y_coords)
            free(y_coords);
        va_end(args);
        return -1;
    }

    for (int i = 0; i < n; i++)
    {
        x_coords[i] = va_arg(args, double);
        y_coords[i] = va_arg(args, double);
    }

    va_end(args);

    int leftmost = 0;
    for (int i = 1; i < n; i++)
    {
        if (x_coords[i] < x_coords[leftmost] ||
            (fabs(x_coords[i] - x_coords[leftmost]) < 1e-10 && y_coords[i] < y_coords[leftmost]))
        {
            leftmost = i;
        }
    }

    int negative = 0, positive = 0;

    for (int i = 0; i < n; i++)
    {
        int curr = i;
        int next = (i + 1) % n;
        int next_next = (i + 2) % n;

        double dx1 = x_coords[next] - x_coords[curr];
        double dy1 = y_coords[next] - y_coords[curr];
        double dx2 = x_coords[next_next] - x_coords[next];
        double dy2 = y_coords[next_next] - y_coords[next];

        double cross = dx1 * dy2 - dy1 * dx2;

        if (fabs(cross) > 1e-10)
        {
            if (cross > 0)
                positive++;
            else
                negative++;

            if (positive > 0 && negative > 0)
            {
                free(x_coords);
                free(y_coords);
                return 0;
            }
        }
    }

    free(x_coords);
    free(y_coords);

    if (positive == 0 && negative == 0)
    {
        return -1;
    }

    return 1;
}

double polynomial_value(double x, int n, ...)
{
    if (n < 0)
    {
        fprintf(stderr, "Error: Invalid polynomial degree.\n");
        return -1.0;
    }

    va_list args;
    va_start(args, n);

    double result = 0.0;
    for (int i = n; i >= 0; --i)
    {
        double coeff = va_arg(args, double);
        result = result * x + coeff;
    }

    va_end(args);
    return result;
}

unsigned long long str_to_base(const char *str, int base)
{
    unsigned long long num = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isalnum(str[i]))
        {
            fprintf(stderr, "Error: Invalid character in string: %c\n", str[i]);
            return ULLONG_MAX;
        }

        int digit;
        if (isdigit(str[i]))
        {
            digit = str[i] - '0';
        }
        else
        {
            digit = toupper(str[i]) - 'A' + 10;
        }

        if (digit >= base)
        {
            fprintf(stderr, "Error: Invalid digit for base %d: %c\n", base, str[i]);
            return ULLONG_MAX;
        }
        if (base > 0 && num > ULLONG_MAX / (unsigned long long)base - (unsigned long long)digit)
        {
            fprintf(stderr, "Error: Overflow detected for base %d\n", base);
            return ULLONG_MAX;
        }
        num = num * (unsigned long long)base + (unsigned long long)digit;
    }
    return num;
}

void kaprekar_numbers(int base, ...)
{
    va_list args;
    va_start(args, base);

    if (base < 2)
    {
        fprintf(stderr, "Error: Invalid base %d, must be >= 2\n", base);
        va_end(args);
        return;
    }

    char *str;
    while ((str = va_arg(args, char *)) != NULL)
    {
        unsigned long long num = 0;
        int i = 0;
        for (; str[i] != '\0'; i++)
        {
            if (!isalnum(str[i]))
            {
                fprintf(stderr, "Error: Invalid character in string: %c\n", str[i]);
                num = ULLONG_MAX;
                break;
            }

            int digit;
            if (isdigit(str[i]))
            {
                digit = str[i] - '0';
            }
            else
            {
                digit = toupper(str[i]) - 'A' + 10;
            }

            if (digit >= base)
            {
                fprintf(stderr, "Error: Invalid digit for base %d: %c\n", base, str[i]);
                num = ULLONG_MAX;
                break;
            }
            if (num > ULLONG_MAX / (unsigned long long)base || (ULLONG_MAX - (unsigned long long)digit) / (unsigned long long)base < num)
            {
                fprintf(stderr, "Error: Overflow detected for base %d\n", base);
                num = ULLONG_MAX;
                break;
            }
            num = num * (unsigned long long)base + (unsigned long long)digit;
        }

        if (num == ULLONG_MAX || num == 0)
        {
            continue;
        }

        unsigned long long sq = num * num;

        char sq_str[100];
        sprintf(sq_str, "%llu", sq);

        size_t sq_len = strlen(sq_str);
        for (size_t i = 1; i < sq_len; i++)
        {
            unsigned long long right_divisor = 1;
            for (size_t j = 0; j < i; j++)
            {
                if (right_divisor > ULLONG_MAX / (unsigned long long)base)
                {
                    fprintf(stderr, "Error: Overflow detected for base %d, power %zu\n", base, i);
                    right_divisor = 0;
                    break;
                }
                right_divisor *= (unsigned long long)base;
            }
            if (right_divisor == 0)
            {
                continue;
            }
            unsigned long long right = sq % right_divisor;
            unsigned long long left = sq / right_divisor;

            if (left + right == num && right > 0)
            {
                printf("Kaprekar number: %s (base %d)\n", str, base);
                break;
            }
        }
    }
    va_end(args);
}

int main()
{

    printf("Square: %d\n", is_convex(4,
                                     0.0, 0.0,
                                     2.0, 0.0,
                                     2.0, 2.0,
                                     0.0, 2.0));

    // Test 2: Quadrilateral
    printf("Quadrilateral: %d\n", is_convex(4,
                                            0.0, 0.0,
                                            2.0, 0.0,
                                            1.0, 0.5,
                                            0.0, 2.0));

    // Test 3: Triangle
    printf("Triangle: %d\n", is_convex(3,
                                       0.0, 0.0,
                                       2.0, 0.0,
                                       1.0, 2.0));

    // Test 4: Pentagon with concavity
    printf("Pentagon: %d\n", is_convex(5,
                                       0.0, 0.0,
                                       2.0, 0.0,
                                       2.0, 2.0,
                                       1.0, 1.0,
                                       0.0, 2.0));

    // Test 5: Collinear points
    printf("Collinear points: %d\n", is_convex(3,
                                               0.0, 0.0,
                                               1.0, 1.0,
                                               2.0, 2.0));

    double value = polynomial_value(2, 2, 1.0, 2.0, 3.0);
    printf("Polynomial value: %f\n", value);

    kaprekar_numbers(10, "1", "45", "9", "297", NULL);
    kaprekar_numbers(17, "A", "10", NULL);
    kaprekar_numbers(1, "1", NULL);
    kaprekar_numbers(10, "0", NULL);

    return 0;
}