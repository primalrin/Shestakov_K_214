#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

enum status_code
{
    STATUS_OK,
    STATUS_ERROR_NAN,
    STATUS_ERROR_ARGS,
    STATUS_ERROR_MEMORY,
};

void print_error(enum status_code code)
{
    switch (code)
    {
    case STATUS_ERROR_NAN:
        fprintf(stderr, "Ошибка: Некорректное число (NaN).\n");
        break;
    case STATUS_ERROR_ARGS:
        fprintf(stderr, "Ошибка: Некорректные аргументы функции.\n");
        break;
    case STATUS_ERROR_MEMORY:
        fprintf(stderr, "Ошибка: Не удалось выделить память.\n");
        break;
    default:
        fprintf(stderr, "Ошибка: Неизвестный код ошибки.\n");
        break;
    }
}

double calculate_polynomial(int n, const double *coeffs, double x)
{
    double result = 0.0;
    for (int i = n; i >= 0; i--)
    {
        result = result * x + coeffs[i];
    }
    return result;
}

double calc_derivative(int i, double a, int n, const double *coeffs)
{
    double result = 0.0;
    for (int j = i; j <= n; j++)
    {
        double term = coeffs[j];
        for (int k = 0; k < i; k++)
        {
            term *= (j - k);
        }
        result += term * pow(a, j - i);
    }
    return result;
}

unsigned long long int factorial(unsigned int n)
{
    unsigned long long int res = 1;
    for (unsigned int i = 1; i <= n; ++i)
    {
        res *= i;
    }
    return res;
}

enum status_code decompose_polynomial(double epsilon, double a, double **result_coeffs, int n, ...)
{
    if (epsilon < 0 || result_coeffs == NULL || n < 0)
    {
        return STATUS_ERROR_ARGS;
    }

    double *coeffs = (double *)malloc(sizeof(double) * (unsigned long long int)(n + 1));
    if (coeffs == NULL)
    {
        return STATUS_ERROR_MEMORY;
    }

    va_list args;
    va_start(args, n);
    for (int i = 0; i <= n; i++)
    {
        coeffs[i] = va_arg(args, double);
        if (isnan(coeffs[i]))
        {
            va_end(args);
            free(coeffs);
            return STATUS_ERROR_NAN;
        }
    }
    va_end(args);

    *result_coeffs = (double *)malloc(sizeof(double) * (unsigned long long int)(n + 1));
    if (*result_coeffs == NULL)
    {
        free(coeffs);
        return STATUS_ERROR_MEMORY;
    }

    for (int i = 0; i <= n; i++)
    {
        (*result_coeffs)[i] = calc_derivative(i, a, n, coeffs) / (double)factorial((unsigned int)i);

        if (fabs((*result_coeffs)[i]) < epsilon)
        {
            (*result_coeffs)[i] = 0.0;
        }
    }

    free(coeffs);
    return STATUS_OK;
}

int main()
{
    int n = 2;
    double epsilon = 0.00001;
    double a = 1.0;
    double *g_coeffs = NULL;

    enum status_code status = decompose_polynomial(epsilon, a, &g_coeffs, n, 2.0, 3.0, 4.0);
    if (status != STATUS_OK)
    {
        print_error(status);
        if (g_coeffs)
            free(g_coeffs);
        return (int)status;
    }

    double f_coeffs[] = {2.0, 3.0, 4.0}; // f(x) = 2x^2 + 3x + 4

    double test_points[] = {0.0, 1.0, 2.0, 3.0};
    int num_points = sizeof(test_points) / sizeof(test_points[0]);

    printf("f(x) coefficients: ");
    for (int i = 0; i <= n; i++)
    {
        printf("%.2f ", f_coeffs[i]);
    }
    printf("\n");

    printf("g(x) coefficients: ");
    for (int i = 0; i <= n; i++)
    {
        printf("%.2f ", g_coeffs[i]);
    }
    printf("\n");
    printf("a = %.2f\n", a);
    printf("----------------------------------------\n");
    printf("   x  |   f(x)    |   g(x-a)   | Difference\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < num_points; i++)
    {
        double x = test_points[i];
        double fx = calculate_polynomial(n, f_coeffs, x);
        double gx = calculate_polynomial(n, g_coeffs, x - a);
        double diff = fabs(fx - gx);

        printf("%5.2f | %8.4f | %8.4f | %8.4f\n", x, fx, gx, diff);
    }
    printf("----------------------------------------\n");

    free(g_coeffs);
    return 0;
}