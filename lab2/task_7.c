#include <stdio.h>
#include <math.h>

// Коды ошибок для функции dichotomy
typedef enum
{
    DICHOTOMY_SUCCESS = 0,
    DICHOTOMY_INVALID_INTERVAL,
    DICHOTOMY_INVALID_EPSILON
} dichotomy_status;

// Функция для нахождения корня уравнения методом дихотомии
dichotomy_status dichotomy(double a, double b, double eps, double (*f)(double), double *root)
{
    if (eps <= 0)
    {
        return DICHOTOMY_INVALID_EPSILON;
    }
    if (f(a) * f(b) > 0)
    {
        return DICHOTOMY_INVALID_INTERVAL;
    }

    double c;
    while (fabs(b - a) > eps)
    {
        c = (a + b) / 2;
        if (f(a) * f(c) < 0)
        {
            b = c;
        }
        else
        {
            a = c;
        }
    }
    *root = c;
    return DICHOTOMY_SUCCESS;
}

// Пример функции для уравнения x^2 - 4 = 0
double f1(double x)
{
    return x * x - 4;
}

// Пример функции для уравнения sin(x) = 0
double f2(double x)
{
    return sin(x);
}

int main()
{
    double a1 = 0, b1 = 3, eps1 = 0.001, root1;
    dichotomy_status status1 = dichotomy(a1, b1, eps1, f1, &root1);
    if (status1 == DICHOTOMY_SUCCESS)
    {
        printf("Root of x^2 - 4 = 0 in [%.2f, %.2f] with eps = %.3f: %.4f\n", a1, b1, eps1, root1);
    }
    else if (status1 == DICHOTOMY_INVALID_INTERVAL)
    {
        printf("Error: f(a) and f(b) must have different signs for x^2 - 4 = 0.\n");
    }
    else
    {
        printf("Error: Invalid epsilon value for x^2 - 4 = 0.\n");
    }

    double a2 = 3, b2 = 4, eps2 = 0.0001, root2;
    dichotomy_status status2 = dichotomy(a2, b2, eps2, f2, &root2);
    if (status2 == DICHOTOMY_SUCCESS)
    {
        printf("Root of sin(x) = 0 in [%.2f, %.2f] with eps = %.4f: %.4f\n", a2, b2, eps2, root2);
    }
    else if (status2 == DICHOTOMY_INVALID_INTERVAL)
    {
        printf("Error: f(a) and f(b) must have different signs for sin(x) = 0.\n");
    }
    else
    {
        printf("Error: Invalid epsilon value for sin(x) = 0.\n");
    }

    return 0;
}