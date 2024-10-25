#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Коды ошибок
typedef enum
{
    SUCCESS = 0,
    ERROR_EMPTY_INPUT = -1,
    ERROR_INVALID_CHAR = -2,
    ERROR_OVERFLOW = -3,
    ERROR_INVALID_FORMAT = -4
} ErrorCode;

// Вспомогательные функции для работы с римскими числами
ErrorCode roman_to_decimal(const char *roman, int *result)
{
    if (!roman || !result || !*roman)
    {
        return ERROR_EMPTY_INPUT;
    }

    int values[256] = {0};
    values['I'] = 1;
    values['V'] = 5;
    values['X'] = 10;
    values['L'] = 50;
    values['C'] = 100;
    values['D'] = 500;
    values['M'] = 1000;

    *result = 0;
    int prev_value = 0;

    for (int i = strlen(roman) - 1; i >= 0; i--)
    {
        if (!values[roman[i]])
        {
            return ERROR_INVALID_CHAR;
        }
        int current_value = values[roman[i]];
        if (current_value >= prev_value)
        {
            *result += current_value;
        }
        else
        {
            *result -= current_value;
        }
        prev_value = current_value;
    }

    return SUCCESS;
}

// Функция для получения n-ного числа Фибоначчи
ErrorCode fib(int n, unsigned int *result)
{
    if (!result)
    {
        return ERROR_INVALID_FORMAT;
    }
    if (n < 0)
    {
        return ERROR_INVALID_FORMAT;
    }
    if (n <= 1)
    {
        *result = n;
        return SUCCESS;
    }

    unsigned int prev = 0, curr = 1;
    for (int i = 2; i <= n; i++)
    {
        unsigned int next = curr + prev;
        if (next < curr)
        { // Проверка на переполнение
            return ERROR_OVERFLOW;
        }
        prev = curr;
        curr = next;
    }
    *result = curr;
    return SUCCESS;
}

// Функция для преобразования цекендорфова представления в число
ErrorCode zeckendorf_to_decimal(const char *zeck, unsigned int *result)
{
    if (!zeck || !result || !*zeck)
    {
        return ERROR_EMPTY_INPUT;
    }

    int len = strlen(zeck);
    if (zeck[len - 1] != '1')
    { // Проверка на завершающую единицу
        return ERROR_INVALID_FORMAT;
    }

    *result = 0;
    int fib_index = 0;

    for (int i = 0; i < len - 1; i++)
    {
        if (zeck[i] != '0' && zeck[i] != '1')
        {
            return ERROR_INVALID_CHAR;
        }
        if (zeck[i] == '1')
        {
            unsigned int fib_value;
            ErrorCode err = fib(fib_index + 2, &fib_value);
            if (err != SUCCESS)
            {
                return err;
            }

            unsigned int new_result = *result + fib_value;
            if (new_result < *result)
            { // Проверка на переполнение
                return ERROR_OVERFLOW;
            }
            *result = new_result;
        }
        fib_index++;
    }

    return SUCCESS;
}

// Функция для преобразования строки в число с заданным основанием
ErrorCode string_to_int_base(const char *str, int base, int uppercase, int *result)
{
    if (!str || !result || !*str)
    {
        return ERROR_EMPTY_INPUT;
    }
    if (base < 2 || base > 36)
    {
        base = 10;
    }

    *result = 0;
    int sign = 1;

    if (*str == '-')
    {
        sign = -1;
        str++;
        if (!*str)
        {
            return ERROR_INVALID_FORMAT;
        }
    }

    while (*str)
    {
        int digit;
        if (isdigit(*str))
        {
            digit = *str - '0';
        }
        else
        {
            char c = uppercase ? toupper(*str) : tolower(*str);
            if (!isalpha(c))
            {
                return ERROR_INVALID_CHAR;
            }
            digit = c - (uppercase ? 'A' : 'a') + 10;
        }

        if (digit >= base)
        {
            return ERROR_INVALID_CHAR;
        }

        // Проверка на переполнение перед вычислением
        if (*result > INT_MAX / base ||
            (*result == INT_MAX / base && digit > INT_MAX % base))
        {
            return ERROR_OVERFLOW;
        }

        *result = *result * base + digit;
        str++;
    }

    *result *= sign;
    return SUCCESS;
}

// Основная функция для чтения форматированного ввода
int read_formatted(void *stream, const char *format, va_list args, int is_file)
{
    if (!stream || !format)
    {
        return ERROR_EMPTY_INPUT;
    }

    int items_read = 0;
    char buffer[1024];
    int buffer_pos = 0;

    while (*format)
    {
        if (*format == '%')
        {
            format++;

            if (strncmp(format, "Ro", 2) == 0)
            {
                int *target = va_arg(args, int *);
                buffer_pos = 0;

                while (1)
                {
                    int c = is_file ? fgetc((FILE *)stream) : *(char *)((const char *)stream + buffer_pos);
                    if (!c || !strchr("IVXLCDM", c))
                        break;
                    buffer[buffer_pos++] = c;
                }
                if (buffer_pos == 0)
                    continue;

                buffer[buffer_pos] = '\0';
                if (roman_to_decimal(buffer, target) == SUCCESS)
                {
                    items_read++;
                }
                format += 2;
            }
            else if (strncmp(format, "Zr", 2) == 0)
            {
                unsigned int *target = va_arg(args, unsigned int *);
                buffer_pos = 0;

                while (1)
                {
                    int c = is_file ? fgetc((FILE *)stream) : *(char *)((const char *)stream + buffer_pos);
                    if (!c || (c != '0' && c != '1'))
                        break;
                    buffer[buffer_pos++] = c;
                }
                if (buffer_pos == 0)
                    continue;

                buffer[buffer_pos] = '\0';
                if (zeckendorf_to_decimal(buffer, target) == SUCCESS)
                {
                    items_read++;
                }
                format += 2;
            }
            else if (strncmp(format, "Cv", 2) == 0 || strncmp(format, "CV", 2) == 0)
            {
                int *target = va_arg(args, int *);
                int base = va_arg(args, int);
                buffer_pos = 0;
                int is_uppercase = (format[1] == 'V');

                while (1)
                {
                    int c = is_file ? fgetc((FILE *)stream) : *(char *)((const char *)stream + buffer_pos);
                    if (!isalnum(c))
                        break;
                    buffer[buffer_pos++] = c;
                }
                if (buffer_pos == 0)
                    continue;

                buffer[buffer_pos] = '\0';
                if (string_to_int_base(buffer, base, is_uppercase, target) == SUCCESS)
                {
                    items_read++;
                }
                format += 2;
            }
            else
            {
                char fmt[3] = {'%', *format, '\0'};
                if (is_file)
                {
                    items_read += fscanf((FILE *)stream, fmt, va_arg(args, void *));
                }
                else
                {
                    items_read += sscanf((char *)stream, fmt, va_arg(args, void *));
                }
                format++;
            }
        }
        else
        {
            format++;
        }
    }

    return items_read;
}

int overfscanf(FILE *stream, const char *format, ...)
{
    if (!stream || !format)
    {
        return ERROR_EMPTY_INPUT;
    }

    va_list args;
    va_start(args, format);
    int result = read_formatted(stream, format, args, 1);
    va_end(args);
    return result;
}

int oversscanf(const char *str, const char *format, ...)
{
    if (!str || !format)
    {
        return ERROR_EMPTY_INPUT;
    }

    va_list args;
    va_start(args, format);
    int result = read_formatted((void *)str, format, args, 0);
    va_end(args);
    return result;
}

int main()
{
    // Тест римских чисел
    printf("Testing Roman numerals:\n");
    FILE *fp = fopen("test.txt", "w");
    fprintf(fp, "XIV");
    fclose(fp);

    fp = fopen("test.txt", "r");
    int roman_number;
    overfscanf(fp, "%Ro", &roman_number);
    printf("Roman number XIV = %d\n", roman_number);
    fclose(fp);

    // Тест цекендорфова представления
    printf("\nTesting Zeckendorf representation:\n");
    unsigned int zeck_number;
    oversscanf("10011", "%Zr", &zeck_number);
    printf("Zeckendorf number 10011 = %u\n", zeck_number);

    // Тест произвольных систем счисления
    printf("\nTesting custom base numbers:\n");
    int base_number;
    int base = 16;
    oversscanf("ff", "%Cv", &base_number, base);
    printf("Number ff in base %d = %d\n", base, base_number);

    oversscanf("FF", "%CV", &base_number, base);
    printf("Number FF in base %d = %d\n", base, base_number);

    return 0;
}