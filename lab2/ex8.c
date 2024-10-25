#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

// Преобразование символа в число
int charToValue(char c)
{
    if (isdigit(c))
        return c - '0';
    else if (isalpha(c))
        return toupper(c) - 'A' + 10;
    return -1;
}

// Преобразование числа в символ
char valueToChar(int v)
{
    if (v >= 0 && v <= 9)
        return '0' + v;
    else if (v >= 10 && v < 36)
        return 'A' + v - 10;
    return '?';
}

// Функция сложения двух строковых чисел в столбик в заданной системе счисления
char *addInColumn(const char *num1, const char *num2, int base)
{
    if (!num1 || !num2 || base < 2 || base > 36)
    {
        return NULL;
    }

    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int maxLen = (len1 > len2) ? len1 : len2;

    char *result = (char *)calloc(maxLen + 2, sizeof(char));
    if (!result)
    {
        return NULL;
    }

    int carry = 0;
    int pos1 = len1 - 1, pos2 = len2 - 1, posRes = maxLen;

    while (pos1 >= 0 || pos2 >= 0 || carry)
    {
        int val1 = (pos1 >= 0) ? charToValue(num1[pos1--]) : 0;
        int val2 = (pos2 >= 0) ? charToValue(num2[pos2--]) : 0;

        if (val1 == -1 || val2 == -1 || val1 >= base || val2 >= base)
        {
            free(result);
            return NULL;
        }

        int sum = val1 + val2 + carry;
        carry = sum / base;
        result[posRes--] = valueToChar(sum % base);
    }

    if (posRes < maxLen)
    {
        memmove(result, result + posRes + 1, maxLen - posRes);
        result[maxLen - posRes] = '\0';
    }
    else
    {
        result[maxLen + 1] = '\0';
    }

    char *trimmed = result;
    while (*trimmed == '0' && *(trimmed + 1) != '\0')
    {
        trimmed++;
    }

    if (trimmed != result)
    {
        char *final = strdup(trimmed);
        free(result);
        return final;
    }

    return result;
}

// Основная функция суммирования чисел с переменным числом аргументов
char *sumNumbers(int base, int count, ...)
{
    if (base < 2 || base > 36 || count < 1)
    {
        return NULL;
    }

    va_list args;
    va_start(args, count);

    char *sum = strdup("0");
    if (!sum)
    {
        va_end(args);
        return NULL;
    }

    for (int i = 0; i < count; ++i)
    {
        char *num = va_arg(args, char *);
        if (!num)
        {
            free(sum);
            va_end(args);
            return NULL;
        }

        for (int j = 0; num[j] != '\0'; ++j)
        {
            if (charToValue(num[j]) == -1 || charToValue(num[j]) >= base)
            {
                free(sum);
                va_end(args);
                return NULL;
            }
        }

        char *newSum = addInColumn(sum, num, base);
        if (!newSum)
        {
            free(sum);
            va_end(args);
            return NULL;
        }

        free(sum);
        sum = newSum;
    }

    va_end(args);
    return sum;
}

int main()
{
    char *result = sumNumbers(16, 3, "FF", "1", "FF");
    if (result)
    {
        printf("Sum: %s\n", result);
        free(result);
        return 0;
    }
    else
    {
        printf("Error: Invalid input or calculation error.\n");
        return 1; // Возвращаем ненулевое значение в случае ошибки
    }
}