#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

// Проверяет, имеет ли число конечное представление в системе счисления с заданным основанием
bool is_finite_representation(double number, int base)
{
    if (base < 2)
    {
        return false; // Некорректное основание системы счисления
    }
    double fractional_part = number;
    // Максимальное количество итераций для предотвращения зацикливания
    const int MAX_ITERATIONS = 1000;
    int iterations = 0;

    while (fractional_part > 1e-15 && iterations < MAX_ITERATIONS)
    {
        fractional_part *= base;
        int integer_part = (int)fractional_part;
        fractional_part -= integer_part;
        iterations++;
    }

    return fractional_part < 1e-15;
}

// Проверяет числа на конечное представление в заданной системе счисления
int check_numbers(int base, int count, ...)
{
    if (base < 2)
    {
        printf("Error: Base must be greater than or equal to 2.\n");
        return 1;
    }
    if (count < 0)
    {
        printf("Error: Invalid count of numbers.\n");
        return 1;
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
            return 1;
        }

        if (is_finite_representation(number, base))
        {
            printf("Number %.6f has a finite representation in base %d.\n", number, base);
        }
        else
        {
            printf("Number %.6f does not have a finite representation in base %d.\n", number, base);
        }
    }

    va_end(args);
    return 0;
}

int main()
{
    printf("Test 1: Binary system (base 2)\n");
    check_numbers(2, 3, 0.5, 0.25, 0.125);

    printf("\nTest 2: Decimal system (base 10)\n");
    check_numbers(10, 2, 0.1, 0.3333);

    printf("\nTest 3: Ternary system (base 3)\n");
    check_numbers(3, 2, 1.0 / 3.0, 0.2);

    return 0;
}