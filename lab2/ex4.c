#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// Функция для определения выпуклости многоугольника
int is_convex(int n, ...)
{
    // Проверка на минимальное количество вершин
    if (n < 3)
    {
        fprintf(stderr, "Error: Less than three vertices provided.\n");
        return -1;
    }

    va_list args;
    va_start(args, n);

    // Выделение памяти для координат
    double *x_coords = (double *)malloc(n * sizeof(double));
    double *y_coords = (double *)malloc(n * sizeof(double));

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

    // Считывание координат из аргументов
    for (int i = 0; i < n; i++)
    {
        x_coords[i] = va_arg(args, double);
        y_coords[i] = va_arg(args, double);
    }

    va_end(args);

    // Находим самую левую нижнюю точку
    int leftmost = 0;
    for (int i = 1; i < n; i++)
    {
        if (x_coords[i] < x_coords[leftmost] ||
            (fabs(x_coords[i] - x_coords[leftmost]) < 1e-10 && y_coords[i] < y_coords[leftmost]))
        {
            leftmost = i;
        }
    }

    // Проверяем каждый угол многоугольника
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

        // Вычисляем ориентацию трех последовательных точек
        double cross = dx1 * dy2 - dy1 * dx2;

        if (fabs(cross) > 1e-10)
        { // Игнорируем почти коллинеарные точки
            if (cross > 0)
                positive++;
            else
                negative++;

            // Если есть углы разных знаков, многоугольник не выпуклый
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

    // Если все точки коллинеарны
    if (positive == 0 && negative == 0)
    {
        return -1;
    }

    return 1;
}

// Функция для вычисления значения многочлена
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

// Функция для преобразования строки в число в заданной системе счисления
unsigned long long str_to_base(const char *str, int base)
{
    unsigned long long num = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isalnum(str[i]))
        {
            fprintf(stderr, "Error: Invalid character in string: %c\n", str[i]);
            return ULLONG_MAX; // Indicate an error
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
            return ULLONG_MAX; // Indicate an error
        }
        num = num * base + digit;
    }
    return num;
}

// Функция для проверки чисел Капрекара
void kaprekar_numbers(int base, ...)
{
    va_list args;
    va_start(args, base);

    char *str;
    while ((str = va_arg(args, char *)) != NULL)
    {
        unsigned long long num = str_to_base(str, base);
        if (num == ULLONG_MAX)
        { // error
            continue;
        }

        unsigned long long sq = num * num;

        char sq_str[100]; // Adjust size as needed
        sprintf(sq_str, "%llu", sq);

        int sq_len = strlen(sq_str);
        for (int i = 1; i < sq_len; i++)
        {
            unsigned long long right = sq % (unsigned long long)pow(base, i);
            unsigned long long left = sq / (unsigned long long)pow(base, i);

            if (left + right == num && right > 0)
            {
                printf("Kaprekar number: %s (base %d)\n", str, base);
                break; // Found a split, no need to check further
            }
        }
    }
    va_end(args);
}

int main()
{
    // Пример использования is_convex
    // Тест 1: Выпуклый четырехугольник (квадрат)
    printf("Square: %d\n", is_convex(4,
                                     0.0, 0.0,
                                     2.0, 0.0,
                                     2.0, 2.0,
                                     0.0, 2.0));

    // Тест 2: Невыпуклый четырехугольник (вогнутый)
    printf("Concave quadrilateral: %d\n", is_convex(4,
                                                    0.0, 0.0,
                                                    2.0, 0.0,
                                                    1.0, 0.5, // Эта точка делает фигуру невыпуклой
                                                    0.0, 2.0));

    // Тест 3: Треугольник
    printf("Triangle: %d\n", is_convex(3,
                                       0.0, 0.0,
                                       2.0, 0.0,
                                       1.0, 2.0));

    // Тест 4: Пятиугольник с вогнутостью
    printf("Concave pentagon: %d\n", is_convex(5,
                                               0.0, 0.0,
                                               2.0, 0.0,
                                               2.0, 2.0,
                                               1.0, 1.0, // Эта точка создает вогнутость
                                               0.0, 2.0));

    // Тест 5: Коллинеарные точки
    printf("Collinear points: %d\n", is_convex(3,
                                               0.0, 0.0,
                                               1.0, 1.0,
                                               2.0, 2.0));

    // Пример использования polynomial_value
    double value = polynomial_value(2, 2, 1.0, 2.0, 3.0);
    printf("Polynomial value: %f\n", value);

    // Пример использования kaprekar_numbers
    kaprekar_numbers(10, "1", "45", "9", "297", NULL);
    kaprekar_numbers(17, "A", "10", NULL);

    return 0;
}
