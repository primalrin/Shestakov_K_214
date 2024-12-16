#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

typedef enum
{
    SUCCESS = 0,
    ERROR_INVALID_ARGUMENTS = -1,
    ERROR_CALC_STRING_LENGTH = -2,
    ERROR_MEMORY_ALLOCATION = -3,
    ERROR_INVALID_SEED = -4,
    ERROR_REVERSING_STRING = -5,
    ERROR_UPPERCASING_STRING = -6,
    ERROR_REARRANGING_STRING = -7,
    ERROR_CONCATENATING_STRING = -8,
    ERROR_INVALID_FLAG = -9,
    ERROR_OVERFLOW = -10,
    ERROR_INVALID_STRING = -11
} error_code;

int my_strcmp(const char *s1, const char *s2)
{
    if (!s1 || !s2)
    {
        return -1;
    }
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

error_code string_length(const char *str, int *length)
{
    if (!str)
    {
        fprintf(stderr, "Error: Invalid string pointer in string_length.\n");
        return ERROR_INVALID_STRING;
    }
    *length = 0;
    while (str[*length] != '\0')
    {
        (*length)++;
        if (*length == INT_MAX)
        {
            fprintf(stderr, "Error: String length exceeds maximum limit in string_length.\n");
            return ERROR_OVERFLOW;
        }
    }
    return SUCCESS;
}

error_code reverse_string(const char *str, char **reversed_str)
{
    if (!str || !reversed_str)
    {
        fprintf(stderr, "Error: Invalid arguments in reverse_string.\n");
        return ERROR_INVALID_ARGUMENTS;
    }
    int length;
    error_code len_res = string_length(str, &length);
    if (len_res != SUCCESS)
    {
        return len_res;
    }

    *reversed_str = (char *)malloc(sizeof(char) * ((size_t)length + 1));
    if (!*reversed_str)
    {
        perror("Error: Memory allocation failed in reverse_string");
        return ERROR_MEMORY_ALLOCATION;
    }

    for (int i = 0; i < length; i++)
    {
        (*reversed_str)[i] = str[length - 1 - i];
    }
    (*reversed_str)[length] = '\0';
    return SUCCESS;
}

error_code uppercase_odd(const char *str, char **uppercased_str)
{
    if (!str || !uppercased_str)
    {
        fprintf(stderr, "Error: Invalid arguments in uppercase_odd.\n");
        return ERROR_INVALID_ARGUMENTS;
    }

    int length;
    error_code len_res = string_length(str, &length);
    if (len_res != SUCCESS)
    {
        return len_res;
    }

    *uppercased_str = (char *)malloc(sizeof(char) * ((size_t)length + 1));
    if (!*uppercased_str)
    {
        perror("Error: Memory allocation failed in uppercase_odd");
        return ERROR_MEMORY_ALLOCATION;
    }

    for (int i = 0; i < length; i++)
    {
        if (i % 2 != 0)
        {
            (*uppercased_str)[i] = (char)toupper((unsigned char)str[i]);
        }
        else
        {
            (*uppercased_str)[i] = str[i];
        }
    }
    (*uppercased_str)[length] = '\0';
    return SUCCESS;
}

error_code rearrange_string(const char *str, char **rearranged_str)
{
    if (!str || !rearranged_str)
    {
        fprintf(stderr, "Error: Invalid arguments in rearrange_string.\n");
        return ERROR_INVALID_ARGUMENTS;
    }

    int length;
    error_code len_res = string_length(str, &length);
    if (len_res != SUCCESS)
    {
        return len_res;
    }

    *rearranged_str = (char *)malloc(sizeof(char) * ((size_t)length + 1));
    if (!*rearranged_str)
    {
        perror("Error: Memory allocation failed in rearrange_string");
        return ERROR_MEMORY_ALLOCATION;
    }

    int digits_index = 0;
    int letters_index = 0;
    int other_index = 0;

    char *digits = (char *)malloc(sizeof(char) * ((size_t)length + 1));
    char *letters = (char *)malloc(sizeof(char) * ((size_t)length + 1));
    char *other = (char *)malloc(sizeof(char) * ((size_t)length + 1));

    if (!digits || !letters || !other)
    {
        free(digits);
        free(letters);
        free(other);
        free(*rearranged_str);
        perror("Error: Memory allocation failed in rearrange_string");
        return ERROR_MEMORY_ALLOCATION;
    }

    for (int i = 0; i < length; i++)
    {
        if (isdigit((unsigned char)str[i]))
        {
            digits[digits_index++] = str[i];
        }
        else if (isalpha((unsigned char)str[i]))
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

    return SUCCESS;
}
error_code validate_seed(const char *seed_str, unsigned int *seed)
{
    if (!seed_str || !seed)
    {
        fprintf(stderr, "Error: Invalid arguments in validate_seed.\n");
        return ERROR_INVALID_ARGUMENTS;
    }
    char *endptr;
    unsigned long converted_seed = strtoul(seed_str, &endptr, 10);

    if (*endptr != '\0')
    {
        fprintf(stderr, "Error: Invalid seed value: %s\n", seed_str);
        return ERROR_INVALID_SEED;
    }

    if (converted_seed > UINT_MAX)
    {
        fprintf(stderr, "Error: Seed value out of range: %s\n", seed_str);
        return ERROR_INVALID_SEED;
    }

    *seed = (unsigned int)converted_seed;
    return SUCCESS;
}

error_code concatenate_strings(int argc, char *argv[], char **concatenated_str)
{
    if (argc < 4 || !argv || !concatenated_str)
    {
        fprintf(stderr, "Error: Invalid arguments in concatenate_strings.\n");
        return ERROR_INVALID_ARGUMENTS;
    }

    unsigned int local_seed;
    error_code seed_res = validate_seed(argv[2], &local_seed);
    if (seed_res != SUCCESS)
    {
        return seed_res;
    }

    srand(local_seed);
    int total_length = 0;
    for (int i = 3; i < argc; i++)
    {
        int len_res_local;
        error_code len_res = string_length(argv[i], &len_res_local);
        if (len_res != SUCCESS)
        {
            return len_res;
        }
        total_length += len_res_local;
        if (total_length < len_res_local)
        {
            fprintf(stderr, "Error: Integer overflow detected in concatenate_strings.\n");
            return ERROR_OVERFLOW;
        }
    }

    *concatenated_str = (char *)malloc(sizeof(char) * ((size_t)total_length + 1));
    if (!*concatenated_str)
    {
        perror("Error: Memory allocation failed in concatenate_strings");
        return ERROR_MEMORY_ALLOCATION;
    }

    int current_index = 0;
    int *indices = (int *)malloc(sizeof(int) * ((size_t)(argc - 3)));
    if (!indices)
    {
        free(*concatenated_str);
        perror("Error: Memory allocation failed for indices in concatenate_strings");
        return ERROR_MEMORY_ALLOCATION;
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
        int str_len;
        error_code str_len_res = string_length(current_str, &str_len);
        if (str_len_res != SUCCESS)
        {
            free(indices);
            free(*concatenated_str);
            return str_len_res;
        }
        for (int j = 0; j < str_len; j++)
        {
            (*concatenated_str)[current_index++] = current_str[j];
        }
    }

    (*concatenated_str)[current_index] = '\0';
    free(indices);
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <flag> [string] [<unsigned int> <strings...>]\n", argv[0]);
        return ERROR_INVALID_ARGUMENTS;
    }

    char *result_str = NULL;
    error_code result_code = SUCCESS;

    if (my_strcmp(argv[1], "-l") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s -l <string>\n", argv[0]);
            return ERROR_INVALID_ARGUMENTS;
        }
        int length;
        result_code = string_length(argv[2], &length);
        if (result_code != SUCCESS)
        {
            fprintf(stderr, "Error calculating string length.\n");
            return result_code;
        }
        printf("String length: %d\n", length);
    }
    else if (my_strcmp(argv[1], "-r") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s -r <string>\n", argv[0]);
            return ERROR_INVALID_ARGUMENTS;
        }
        result_code = reverse_string(argv[2], &result_str);
        if (result_code != SUCCESS)
        {
            return result_code;
        }
        printf("Reversed string: %s\n", result_str);
    }
    else if (my_strcmp(argv[1], "-u") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s -u <string>\n", argv[0]);
            return ERROR_INVALID_ARGUMENTS;
        }
        result_code = uppercase_odd(argv[2], &result_str);
        if (result_code != SUCCESS)
        {
            return result_code;
        }
        printf("Uppercased odd string: %s\n", result_str);
    }
    else if (my_strcmp(argv[1], "-n") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s -n <string>\n", argv[0]);
            return ERROR_INVALID_ARGUMENTS;
        }
        result_code = rearrange_string(argv[2], &result_str);
        if (result_code != SUCCESS)
        {
            return result_code;
        }
        printf("Rearranged string: %s\n", result_str);
    }
    else if (my_strcmp(argv[1], "-c") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "Usage: %s -c <seed> <string1> <string2> ...\n", argv[0]);
            return ERROR_INVALID_ARGUMENTS;
        }

        char *concatenated_str = NULL;
        result_code = concatenate_strings(argc, argv, &concatenated_str);

        if (result_code != SUCCESS)
        {
            return result_code;
        }
        printf("Concatenated string: %s\n", concatenated_str);
        free(concatenated_str);
    }
    else
    {
        fprintf(stderr, "Error: Invalid flag.\n");
        return ERROR_INVALID_FLAG;
    }

    free(result_str);
    return SUCCESS;
}

//./task_1 -l "Hello, world!"
//./task_1 -r "Reverse me"
//./task_1 -u "Make me uppercase"
//./task_1 -n "DEF?123abc"
//./task_1 -c 12345 "String 1" "String 2" "String 3" "String 4"
