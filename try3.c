#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
    int (*func)(int);
    float coeff;
} Predicate;

int fix(int value)
{
    return value != 0;
}

// Modified to properly handle variable arguments
float calculate_ording(int item, int pred_count, Predicate predicates[])
{
    float ording = 0;
    for (int i = 0; i < pred_count; ++i)
    {
        ording += fix(predicates[i].func(item)) * predicates[i].coeff;
    }
    return ording;
}

typedef struct
{
    int value;
    float ording;
} ArrayItem;

int compare_items(const void *a, const void *b)
{
    ArrayItem *item_a = (ArrayItem *)a;
    ArrayItem *item_b = (ArrayItem *)b;

    if (item_a->ording < item_b->ording)
        return -1;
    if (item_a->ording > item_b->ording)
        return 1;
    return 0;
}

int compare_items_descending(const void *a, const void *b)
{
    return -compare_items(a, b);
}

// Modified to accept array of predicates instead of variable arguments
int sort_by_predicates(int *values, size_t values_count, int pred_count,
                       int **result, int ascending, Predicate predicates[])
{
    if (values == NULL || values_count == 0 || pred_count == 0 ||
        result == NULL || predicates == NULL)
    {
        return -1;
    }

    ArrayItem *items = (ArrayItem *)malloc(values_count * sizeof(ArrayItem));
    if (items == NULL)
    {
        return -2;
    }

    for (size_t i = 0; i < values_count; ++i)
    {
        items[i].value = values[i];
        items[i].ording = calculate_ording(values[i], pred_count, predicates);
    }

    if (ascending)
    {
        qsort(items, values_count, sizeof(ArrayItem), compare_items);
    }
    else
    {
        qsort(items, values_count, sizeof(ArrayItem), compare_items_descending);
    }

    *result = (int *)malloc(values_count * sizeof(int));
    if (*result == NULL)
    {
        free(items);
        return -3;
    }

    for (size_t i = 0; i < values_count; ++i)
    {
        (*result)[i] = items[i].value;
    }

    free(items);
    return 0;
}

int is_even(int num)
{
    return num % 2 == 0;
}

int is_positive(int num)
{
    return num > 0;
}

int main()
{
    int values[] = {-5, 2, 8, -1, 0, 6};
    size_t values_count = sizeof(values) / sizeof(values[0]);
    int *sorted_values = NULL;
    int res;

    Predicate predicates[] = {
        {is_even, 1.0f},
        {is_positive, 2.0f}};

    // Ascending sort
    res = sort_by_predicates(values, values_count, 2, &sorted_values, 1, predicates);
    if (res == 0)
    {
        printf("Sorted ascending: ");
        for (size_t i = 0; i < values_count; ++i)
        {
            printf("%d ", sorted_values[i]);
        }
        printf("\n");
        free(sorted_values);
    }

    // Descending sort
    res = sort_by_predicates(values, values_count, 2, &sorted_values, 0, predicates);
    if (res == 0)
    {
        printf("Sorted descending: ");
        for (size_t i = 0; i < values_count; ++i)
        {
            printf("%d ", sorted_values[i]);
        }
        printf("\n");
        free(sorted_values);
    }

    return 0;
}