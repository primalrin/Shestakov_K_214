#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// Функция для подсчета длины строки
int string_length(const char *str)
{
    if (!str)
    {
        return -1; // Код ошибки для NULL строки
    }
    int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    return length;
}

// Функция для реверса строки
int reverse_string(const char *str, char **reversed_str)
{
    if (!str || !reversed_str)
    {
        return -1; // Код ошибки для NULL аргументов
    }
    int length = string_length(str);
    if (length < 0)
    {
        return -2; // Код ошибки, если не удалось вычислить длину
    }

    *reversed_str = (char *)malloc(sizeof(char) * (length + 1));
    if (!*reversed_str)
    {
        perror("Memory allocation failed");
        return -3; // Код ошибки выделения памяти
    }

    for (int i = 0; i < length; i++)
    {
        (*reversed_str)[i] = str[length - 1 - i];
    }
    (*reversed_str)[length] = '\0';
    return 0;
}

// Функция для преобразования символов на нечетных позициях в верхний регистр
int uppercase_odd(const char *str, char **uppercased_str)
{
    if (!str || !uppercased_str)
    {
        return -1;
    }

    int length = string_length(str);
    if (length < 0)
    {
        return -2; // Код ошибки, если не удалось вычислить длину
    }

    *uppercased_str = (char *)malloc(sizeof(char) * (length + 1));
    if (!*uppercased_str)
    {
        perror("Memory allocation failed");
        return -3;
    }

    for (int i = 0; i < length; i++)
    {
        if (i % 2 != 0)
        {
            (*uppercased_str)[i] = toupper(str[i]);
        }
        else
        {
            (*uppercased_str)[i] = str[i];
        }
    }
    (*uppercased_str)[length] = '\0';
    return 0;
}

// Функция для перестановки символов в строке
int rearrange_string(const char *str, char **rearranged_str)
{
    if (!str || !rearranged_str)
    {
        return -1;
    }

    int length = string_length(str);
    if (length < 0)
    {
        return -2; // Код ошибки, если не удалось вычислить длину
    }

    *rearranged_str = (char *)malloc(sizeof(char) * (length + 1));
    if (!*rearranged_str)
    {
        perror("Memory allocation failed");
        return -3;
    }

    int digits_index = 0;
    int letters_index = 0;
    int other_index = 0;

    char *digits = (char *)malloc(sizeof(char) * (length + 1));
    char *letters = (char *)malloc(sizeof(char) * (length + 1));
    char *other = (char *)malloc(sizeof(char) * (length + 1));

    if (!digits || !letters || !other)
    {
        free(digits);
        free(letters);
        free(other);
        free(*rearranged_str);
        perror("Memory allocation failed");
        return -3; // Код ошибки выделения памяти
    }

    for (int i = 0; i < length; i++)
    {
        if (isdigit(str[i]))
        {
            digits[digits_index++] = str[i];
        }
        else if (isalpha(str[i]))
        {
            letters[letters_index++] = str[i];
        }
        else
        {
            other[other_index++] = str[i];
        }
    }
    digits[digits_index] = '\0';
    letters[letters_index] = '\0';
    other[other_index] = '\0';

    int current_index = 0;

    for (int i = 0; i < digits_index; i++, current_index++)
    {
        (*rearranged_str)[current_index] = digits[i];
    }

    for (int i = 0; i < letters_index; i++, current_index++)
    {
        (*rearranged_str)[current_index] = letters[i];
    }

    for (int i = 0; i < other_index; i++, current_index++)
    {
        (*rearranged_str)[current_index] = other[i];
    }

    (*rearranged_str)[current_index] = '\0';

    free(digits);
    free(letters);
    free(other);

    return 0;
}

// Функция для конкатенации строк в случайном порядке
int concatenate_strings(int argc, char *argv[], char **concatenated_str)
{
    if (argc < 4 || !argv || !concatenated_str)
    {
        return -1;
    }

    char *endptr;
    unsigned long converted_seed = strtoul(argv[2], &endptr, 10); // Seed теперь из argv[2]

    if (*endptr != '\0' || converted_seed > UINT_MAX)
    {
        fprintf(stderr, "Invalid seed value: %s\n", argv[2]);
        return -4;
    }

    unsigned int local_seed = (unsigned int)converted_seed;

    srand(local_seed);
    int total_length = 0;
    for (int i = 3; i < argc; i++)
    {
        int len_res = string_length(argv[i]);
        if (len_res < 0)
        {
            return -2;
        }
        total_length += len_res;
    }

    *concatenated_str = (char *)malloc(sizeof(char) * (total_length + 1));
    if (!*concatenated_str)
    {
        perror("Memory allocation failed");
        return -3;
    }

    int current_index = 0;
    int *indices = (int *)malloc(sizeof(int) * (argc - 3));
    if (!indices)
    {
        free(*concatenated_str);
        perror("Failed to allocate memory for indices");
        return -3;
    }

    for (int i = 0; i < argc - 3; i++)
    {
        indices[i] = i + 3;
    }

    for (int i = argc - 4; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < argc - 3; i++)
    {
        int str_index = indices[i];
        const char *current_str = argv[str_index];
        int str_len = string_length(current_str);
        if (str_len < 0)
        {
            free(indices);
            free(*concatenated_str);
            return -2;
        }
        for (int j = 0; j < str_len; j++)
        {
            (*concatenated_str)[current_index++] = current_str[j];
        }
    }

    (*concatenated_str)[current_index] = '\0';
    free(indices);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <flag> <string> [<unsigned int> <strings...>]\n", argv[0]);
        return -1;
    }

    char *result_str = NULL;
    int result_code = 0;

    if (strcmp(argv[1], "-l") == 0)
    {
        int length = string_length(argv[2]);
        if (length < 0)
        {
            fprintf(stderr, "Error calculating string length.\n");
            return -2;
        }
        printf("String length: %d\n", length);
    }
    else if (strcmp(argv[1], "-r") == 0)
    {
        result_code = reverse_string(argv[2], &result_str);
        if (result_code != 0)
        {
            fprintf(stderr, "Error reversing string.\n");
            return -3;
        }
        printf("Reversed string: %s\n", result_str);
    }
    else if (strcmp(argv[1], "-u") == 0)
    {
        result_code = uppercase_odd(argv[2], &result_str);
        if (result_code != 0)
        {
            fprintf(stderr, "Error uppercasing odd characters.\n");
            return -4;
        }
        printf("Uppercased odd string: %s\n", result_str);
    }
    else if (strcmp(argv[1], "-n") == 0)
    {
        result_code = rearrange_string(argv[2], &result_str);
        if (result_code != 0)
        {
            fprintf(stderr, "Error rearranging string.\n");
            return -5;
        }
        printf("Rearranged string: %s\n", result_str);
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "Not enough arguments for -c flag.\n");
            return -6;
        }

        char *concatenated_str = NULL;
        int result_code = concatenate_strings(argc, argv, &concatenated_str);

        if (result_code != 0)
        {
            fprintf(stderr, "Error concatenating strings.\n");
            return -8;
        }
        printf("Concatenated string: %s\n", concatenated_str); // Исправлено: вывод concatenated_str
        free(concatenated_str);                                // Освобождаем память
    }
    else
    {
        fprintf(stderr, "Invalid flag.\n");
        return -9;
    }

    free(result_str);
    return 0;
}

// gcc ex1.c -o ex1

//./ex1 -l "Hello, world!"
//./ex1 -r "Reverse me"
//./ex1 -u "Make me uppercase"
//./ex1 -n "DEF?123abc"
//./ex1 -c 12345 "String 1" "String 2" "String 3" "String 4"