#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

typedef enum
{
    SUCCESS = 0,
    ERROR_EMPTY_INPUT = -1,
    ERROR_INVALID_CHAR = -2,
    ERROR_OVERFLOW = -3,
    ERROR_INVALID_FORMAT = -4
} ErrorCode;

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

    for (int i = (int)strlen(roman) - 1; i >= 0; i--)
    {
        if (!values[(unsigned char)roman[i]])
        {
            return ERROR_INVALID_CHAR;
        }
        int current_value = values[(unsigned char)roman[i]];
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
        *result = (unsigned int)n;
        return SUCCESS;
    }

    unsigned int prev = 0, curr = 1;
    for (int i = 2; i <= n; i++)
    {
        unsigned int next = curr + prev;
        if (next < curr)
        {
            return ERROR_OVERFLOW;
        }
        prev = curr;
        curr = next;
    }
    *result = curr;
    return SUCCESS;
}

ErrorCode zeckendorf_to_decimal(const char *zeck, unsigned int *result)
{
    if (!zeck || !result || !*zeck)
    {
        return ERROR_EMPTY_INPUT;
    }

    int len = (int)strlen(zeck);
    
    *result = 0;
    int fib_index = 2; 

    for (int i = len - 2; i >= 0; i--)
    {
        if (zeck[i] != '0' && zeck[i] != '1')
        {
            return ERROR_INVALID_CHAR;
        }
        if (zeck[i] == '1')
        {
            unsigned int fib_value;
            ErrorCode err = fib(fib_index, &fib_value);
            if (err != SUCCESS)
            {
                return err;
            }

            if (UINT_MAX - fib_value < *result)
            {
                return ERROR_OVERFLOW;
            }

            *result += fib_value;
        }
        fib_index++;
    }
    
    if (zeck[len -1] != '1') return ERROR_INVALID_FORMAT;

    return SUCCESS;
}

ErrorCode string_to_int_base(const char *str, int base, int uppercase, int *result)
{
    if (!str || !result || !*str)
    {
        return ERROR_EMPTY_INPUT;
    }
    if (base < 2 || base > 36)
    {
        return ERROR_INVALID_FORMAT;
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
            int c = uppercase ? toupper((unsigned char)*str) : tolower((unsigned char)*str);
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

int read_formatted(void *stream, const char *format, va_list args, int is_file)
{
    if (!stream || !format)
    {
        return ERROR_EMPTY_INPUT;
    }

    int items_read = 0;
    char buffer[1024];
    int buffer_pos = 0;
    int error_code = SUCCESS;

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
                    int c = is_file ? fgetc((FILE *)stream) : *(unsigned char *)((const char *)stream + buffer_pos);
                    if (c == EOF || !strchr("IVXLCDM", c))
                        break;
                    buffer[buffer_pos++] = (char)c;
                }

                if (is_file) {
                    for (int i = buffer_pos - 1; i >= 0; i--) {
                        ungetc(buffer[i], (FILE *)stream);
                    }
                }

                if (buffer_pos == 0) {
                    format += 2;
                    continue;
                }

                buffer[buffer_pos] = '\0';
                error_code = roman_to_decimal(buffer, target);
                if (error_code == SUCCESS)
                {
                    items_read++;
                }
                else if (error_code == ERROR_INVALID_CHAR)
                {
                    return error_code;
                }
                format += 2;
            }
            else if (strncmp(format, "Zr", 2) == 0)
            {
                unsigned int *target = va_arg(args, unsigned int *);
                buffer_pos = 0;

                while (1)
                {
                    int c = is_file ? fgetc((FILE *)stream) : *(unsigned char *)((const char *)stream + buffer_pos);
                    if (c == EOF || (c != '0' && c != '1'))
                        break;
                    buffer[buffer_pos++] = (char)c;
                }
                
                if (is_file) {
                    for (int i = buffer_pos - 1; i >= 0; i--) {
                        ungetc(buffer[i], (FILE *)stream);
                    }
                }

                if (buffer_pos == 0) {
                    format += 2;
                    continue;
                }

                buffer[buffer_pos++] = '1';
                buffer[buffer_pos] = '\0';

                error_code = zeckendorf_to_decimal(buffer, target);
                if (error_code == SUCCESS)
                {
                    items_read++;
                }
                else if (error_code == ERROR_INVALID_CHAR || error_code == ERROR_INVALID_FORMAT)
                {
                    return error_code;
                }
                format += 2;
            }
            else if (strncmp(format, "Cv", 2) == 0 || strncmp(format, "CV", 2) == 0)
            {
                int *target = va_arg(args, int *);
                int base = va_arg(args, int);

                if (base < 2 || base > 36) {
                    return ERROR_INVALID_FORMAT;
                }

                buffer_pos = 0;
                int is_uppercase = (format[1] == 'V');

                while (1)
                {
                    int c = is_file ? fgetc((FILE *)stream) : *(unsigned char *)((const char *)stream + buffer_pos);
                    if (c == EOF || !isalnum(c))
                        break;
                    buffer[buffer_pos++] = (char)c;
                }

                if (is_file) {
                    for (int i = buffer_pos - 1; i >= 0; i--) {
                        ungetc(buffer[i], (FILE *)stream);
                    }
                }

                if (buffer_pos == 0) {
                    format += 2;
                    continue;
                }

                buffer[buffer_pos] = '\0';
                error_code = string_to_int_base(buffer, base, is_uppercase, target);
                if (error_code == SUCCESS)
                {
                    items_read++;
                }
                else if (error_code == ERROR_INVALID_CHAR)
                {
                    return error_code;
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

    printf("Testing Roman numerals:\n");
    FILE *fp = fopen("test.txt", "w");
    if (!fp)
    {
        perror("Error opening file");
        return 1;
    }
    fprintf(fp, "XIV");
    fclose(fp);

    fp = fopen("test.txt", "r");
    if (!fp)
    {
        perror("Error opening file");
        return 1;
    }
    int roman_number;
    if (overfscanf(fp, "%Ro", &roman_number) == 1)
    {
        printf("Roman number XIV = %d\n", roman_number);
    }
    else
    {
        printf("Failed to read Roman number.\n");
    }
    fclose(fp);

    printf("\nTesting Zeckendorf representation:\n");
    unsigned int zeck_number;
    if (oversscanf("1001", "%Zr", &zeck_number) == 1)
    {
        printf("Zeckendorf number 10011 = %u\n", zeck_number);
    }
    else
    {
        printf("Failed to read Zeckendorf number.\n");
    }

    printf("\nTesting base numbers:\n");
    int base_number;
    int base = 16;
    if (oversscanf("ff", "%Cv", &base_number, base) == 1)
    {
        printf("Number ff in base %d = %d\n", base, base_number);
    }
    else
    {
        printf("Failed to read base number (lowercase).\n");
    }

    if (oversscanf("FF", "%CV", &base_number, base) == 1)
    {
        printf("Number FF in base %d = %d\n", base, base_number);
    }
    else
    {
        printf("Failed to read base number (uppercase).\n");
    }

    printf("\nTesting invalid base:\n");
    if (oversscanf("123", "%Cv", &base_number, 2) == ERROR_INVALID_FORMAT)
    {
        printf("Invalid base 2.\n");
    }
    else
    {
        printf("Failed to detect invalid base.\n");
    }

    return 0;
}
