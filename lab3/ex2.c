#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <float.h>

#define EPS 0.0000001

typedef enum
{
    SUCCESS,
    ERROR_MALLOC,
    ERROR_ARGS,
} ret_type_t;

typedef struct
{
    int N;
    double *Coordinates;
} DimensionVector;

void DestroyDimensionVector(DimensionVector *v)
{
    free(v->Coordinates);
    free(v);
}

DimensionVector *CopyDimensionVector(DimensionVector *v)
{
    DimensionVector *result = malloc(sizeof(DimensionVector));
    double *coord = malloc(sizeof(double) * v->N);
    for (int i = 0; i < v->N; i++)
    {
        coord[i] = v->Coordinates[i];
    }
    result->Coordinates = coord;
    result->N = v->N;
    return result;
}

DimensionVector *NewDimensionVector(int n, ...)
{
    va_list args;
    double *coords = malloc(sizeof(double) * n);
    DimensionVector *result = malloc(sizeof(DimensionVector));
    if (!coords)
    {
        return NULL;
    }

    if (!result)
    {
        free(coords);
        return NULL;
    }

    va_start(args, n);
    for (int i = 0; i < n; i++)
    {
        coords[i] = va_arg(args, double);
    }
    va_end(args);
    result->Coordinates = coords;
    result->N = n;
    return result;
}

typedef struct
{
    int len;
    int capacity;
    DimensionVector **buffer;
} Vector;

Vector *CreateVector(int initialCapacity)
{
    Vector *v = malloc(sizeof(Vector));
    if (!v)
    {
        return NULL;
    }
    v->buffer = malloc(sizeof(DimensionVector *) * initialCapacity);
    if (!v->buffer)
    {
        free(v);
        return NULL;
    }
    v->capacity = initialCapacity;
    v->len = 0;
    return v;
}

void DestroyVector(Vector *v)
{
    for (int i = 0; i < v->len; i++)
    {
        free(v->buffer[i]->Coordinates);
        free(v->buffer[i]);
    }
    free(v->buffer);
    free(v);
};

void VectorPush(Vector *v, DimensionVector *value)
{
    if (v->len + 1 >= v->capacity)
    {
        DimensionVector **newBuffer = malloc(v->capacity * sizeof(DimensionVector *) * 2);
        for (int i = 0; i < v->len; i++)
        {
            newBuffer[i] = v->buffer[i];
        }

        free(v->buffer);
        v->buffer = newBuffer;
        v->capacity *= 2;
    }
    v->buffer[v->len] = value;
    v->len++;
}

void VectorPop(Vector *v)
{
    v->len--;
    free(v->buffer[v->len]->Coordinates);
    free(v->buffer[v->len]);
}

typedef double (*NormaCalculator)(const DimensionVector *, const void *);

double MaxNorma(const DimensionVector *v, const void *arg)
{
    double max = fabs(v->Coordinates[0]);
    for (int i = 1; i < v->N; i++)
    {
        if (fabs(v->Coordinates[i]) > max)
        {
            max = fabs(v->Coordinates[i]);
        }
    }
    return max;
}

double SumNorma(const DimensionVector *v, const void *arg)
{
    double p = *(double *)arg;
    double sum = 0.0;
    for (int i = 0; i < v->N; i++)
    {
        sum += pow(fabs(v->Coordinates[i]), p);
    }
    return pow(sum, 1.0 / p);
}

double MatrixNorma(const DimensionVector *v, const void *arg)
{
    double *A = (double *)arg;
    double sum = 0.0;
    for (int i = 0; i < v->N; i++)
    {
        for (int j = 0; j < v->N; j++)
        {
            sum += v->Coordinates[i] * A[i * v->N + j] * v->Coordinates[j];
        }
    }
    return sqrt(sum);
}

void FreeVectors(Vector **vectors, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (vectors[i])
        {
            DestroyVector(vectors[i]);
        }
    }
    free(vectors);
}

Vector **GetMaxNorma(int *code, int dimension, int numberOfVectors, int numberOfNormas, ...)
{
    va_list args;
    va_start(args, numberOfNormas);

    if (dimension < 0 || numberOfNormas < 0 || numberOfNormas > 3)
    {
        *code = ERROR_ARGS;
        return NULL;
    }

    DimensionVector **vectors = (DimensionVector **)malloc(sizeof(DimensionVector *) * numberOfVectors);
    if (!vectors)
    {
        va_end(args);
        *code = ERROR_MALLOC;
        return NULL;
    }

    Vector **output = (Vector **)malloc(sizeof(Vector *) * numberOfNormas);
    if (!output)
    {
        free(vectors);
        va_end(args);
        *code = ERROR_MALLOC;
        return NULL;
    }

    for (int i = 0; i < numberOfVectors; i++)
    {
        vectors[i] = va_arg(args, DimensionVector *);
    }

    for (int k = 0; k < numberOfNormas; k++)
    {
        output[k] = CreateVector(1);
        if (!output[k])
        {
            FreeVectors(output, k);
            free(vectors);
            va_end(args);
            *code = ERROR_MALLOC;
            return NULL;
        }

        NormaCalculator f = va_arg(args, NormaCalculator);
        void *arg = va_arg(args, void *);
        double longestNorma = -DBL_MAX;

        for (int i = 0; i < numberOfVectors; i++)
        {
            double norma = f(vectors[i], arg);

            if (longestNorma < norma)
            {
                longestNorma = norma;
                DestroyVector(output[k]);
                output[k] = CreateVector(1);
                VectorPush(output[k], CopyDimensionVector(vectors[i]));
            }
            else if (fabs(longestNorma - norma) < EPS)
            {
                VectorPush(output[k], CopyDimensionVector(vectors[i]));
            }
        }
    }
    free(vectors);
    va_end(args);
    *code = SUCCESS;
    return output;
}

int main(void)
{
    printf("Test Case 1: Testing with multiple vectors and all three norms\n");

    DimensionVector *v1 = NewDimensionVector(3, 1.0, 2.0, 3.0);
    DimensionVector *v2 = NewDimensionVector(3, -4.0, 0.0, 0.0);
    DimensionVector *v3 = NewDimensionVector(3, 1.0, 1.0, 1.0);

    if (!v1 || !v2 || !v3)
    {
        printf("Error: Memory allocation failed for DimensionVectors.\n");
        if (v1)
            DestroyDimensionVector(v1);
        if (v2)
            DestroyDimensionVector(v2);
        if (v3)
            DestroyDimensionVector(v3);
        return ERROR_MALLOC;
    }

    int code = SUCCESS;
    int numberOfNormas = 3;
    int numberOfVectors = 3;
    double p = 2.0;
    double A[9] = {1.0, 0.0, 0.0,
                   0.0, 1.0, 0.0,
                   0.0, 0.0, 1.0};

    Vector **result = GetMaxNorma(&code, 3, numberOfVectors, numberOfNormas,
                                  v1, v2, v3,
                                  MaxNorma, NULL,
                                  SumNorma, &p,
                                  MatrixNorma, &A);

    if (code != SUCCESS)
    {
        printf("Error: GetMaxNorma failed with code %d.\n", code);
        DestroyDimensionVector(v1);
        DestroyDimensionVector(v2);
        DestroyDimensionVector(v3);
        return code;
    }

    for (int i = 0; i < numberOfNormas; i++)
    {
        if (i == 0)
        {
            printf("MaxNorma (Infinity Norm):\n");
        }
        else if (i == 1)
        {
            printf("SumNorma (p-Norm with p=%.1f):\n", p);
        }
        else
        {
            printf("MatrixNorma (Euclidean Norm with given matrix A):\n");
        }

        for (int j = 0; j < result[i]->len; j++)
        {
            DimensionVector *v = result[i]->buffer[j];
            printf("  Vector %d: ", j + 1);
            for (int k = 0; k < v->N; k++)
            {
                printf("%.2f ", v->Coordinates[k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    DestroyDimensionVector(v1);
    DestroyDimensionVector(v2);
    DestroyDimensionVector(v3);
    FreeVectors(result, numberOfNormas);

    return 0;
}