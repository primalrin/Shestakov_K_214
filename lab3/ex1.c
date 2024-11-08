#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

// Извлечение r бит
uint32_t get_mask(uint32_t r)
{
    return ~(~0u << r);
}

uint32_t bit_add(uint32_t a, uint32_t b)
{
    uint32_t carry;
    while (b)
    {
        carry = a & b;
        a = a ^ b;
        b = carry << 1;
    }
    return a;
}

uint32_t bit_sub(uint32_t a, uint32_t b)
{
    return bit_add(a, bit_add(~b, 1));
}

// X to Y
char get_digit_char(uint32_t digit, uint32_t r)
{
    const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
    if (digit > strlen(digits))
    {
        return '\0';
    }
    return digits[digit];
}

uint32_t get_length(uint32_t number, uint32_t r)
{
    uint32_t len = 1;
    uint32_t mask = get_mask(r);

    while (number)
    {
        len = bit_add(len, 1);
        number = number >> r;
    }
    return len;
}

int convert_to_base_2r(uint32_t number, uint32_t r, char *result)
{

    if (r == 0 || (r & get_mask(3)) > 5)
    {
        return 1;
    }

    if (!number)
    {
        result[0] = '0';
        result[1] = '\0';
        return 0;
    }

    uint32_t mask = get_mask(r);
    uint32_t len = get_length(number, r);

    uint32_t pos = 0;

    uint32_t min_len;
    switch (r)
    {
    case 1:
        min_len = 8;
        break;
    case 2:
        min_len = 4;
        break;
    case 3:
        min_len = 3;
        break;
    case 4:
        min_len = 2;
        break;
    default:
        min_len = 2;
        break;
    }

    while (len < min_len && len <= UINT32_MAX - 1)
    {
        len = bit_add(len, 1);
    }

    uint32_t temp = number;
    uint32_t digits[32];
    uint32_t digit_count = 0;

    while (temp)
    {
        digits[digit_count] = temp & mask;
        temp = temp >> r;
        if (digit_count < 31)
        {
            digit_count = bit_add(digit_count, 1);
        }
        else
        {
            return 1;
        }
    }

    while (digit_count > 0)
    {
        digit_count = bit_sub(digit_count, 1);
        result[pos] = get_digit_char(digits[digit_count], r);
        if (result[pos] == '\0')
        {
            return 1;
        }

        if (pos < 31)
        {
            pos = bit_add(pos, 1);
        }
        else
        {
            return 1;
        }
    }

    result[pos] = '\0';
    return 0;
}

void demonstrate_conversion(uint32_t number)
{
    char result[33];
    int res;

    printf("Number: %u\n", number);

    for (uint32_t r = 1; r < 6; r = bit_add(r, 1))
    {
        res = convert_to_base_2r(number, r, result);
        if (res == 0)
        {
            printf("Base 2^%u (=%u): %s\n", r, 1u << r, result);
        }
        else
        {
            printf("Conversion error for r = %u\n", r);
        }
    }
    printf("\n");
}

int main()
{
    demonstrate_conversion(255);
    demonstrate_conversion(1000);
    demonstrate_conversion(0);

    return 0;
}