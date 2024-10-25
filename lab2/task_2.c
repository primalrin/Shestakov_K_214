#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <float.h>

// Перечисление для кодов ошибок
typedef enum
{
    SUCCESS,
    INVALID_INPUT,
    NEGATIVE_NUMBER,
    POWER_OVERFLOW
} error_code;

// Функция для вычисления среднего геометрического чисел
error_code geometric_mean(double *result, int count, ...)
{
    if (count <= 0)
    {
        fprintf(stderr, "Error: Number of elements must be positive.\n");
        return INVALID_INPUT;
    }

    va_list args;
    va_start(args, count);

    double product = 1.0;
    for (int i = 0; i < count; ++i)
    {
        double num = va_arg(args, double);
        if (num < 0)
        {
            fprintf(stderr, "Error: Argument %d is negative. Cannot calculate geometric mean of negative numbers.\n", i + 1);
            return NEGATIVE_NUMBER;
        }
        else if (num == 0)
        {
            *result = 0.0;
            return SUCCESS;
        }
        product *= num;
    }

    va_end(args);

    *result = pow(product, 1.0 / count);
    return SUCCESS;
}

// Рекурсивная функция быстрого возведения в степень
error_code power(double *result, double base, int exp)
{
    if (exp == 0)
    {
        *result = 1.0;
        return SUCCESS;
    }
    else if (exp < 0)
    {
        error_code err = power(result, base, -exp);
        if (err != SUCCESS)
        {
            return err;
        }
        *result = 1.0 / (*result);
        return SUCCESS;
    }
    else if (exp % 2 == 0)
    {
        double temp;
        error_code err = power(&temp, base, exp / 2);
        if (err != SUCCESS)
        {
            return err;
        }
        if (temp > DBL_MAX / temp)
        {
            fprintf(stderr, "Error: Overflow in power function.\n");
            return POWER_OVERFLOW;
        }
        *result = temp * temp;
        return SUCCESS;
    }
    else
    {
        double temp;
        error_code err = power(&temp, base, exp - 1);
        if (err != SUCCESS)
        {
            return err;
        }
        if (base > DBL_MAX / temp)
        {
            fprintf(stderr, "Error: Overflow in power function.\n");
            return POWER_OVERFLOW;
        }
        *result = base * temp;
        return SUCCESS;
    }
}

int main()
{
    double gm;
    error_code err = geometric_mean(&gm, 3, 2.0, 4.0, 8.0);
    if (err != SUCCESS)
    {
        return 1;
    }
    printf("Geometric mean: %f\n", gm);

    double pow_result;
    err = power(&pow_result, 2.0, 3);
    if (err != SUCCESS)
    {
        return 1;
    }
    printf("2^3: %f\n", pow_result);

    err = power(&pow_result, 3.0, -2);
    if (err != SUCCESS)
    {
        return 1;
    }
    printf("3^-2: %f\n", pow_result);

    err = geometric_mean(&gm, 0);
    if (err != SUCCESS)
    {
        return 1;
    }

    return 0;
}