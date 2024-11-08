#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#define EPS 0.0000001

// co
// SUCCESS 0
// ERROR_MALLOC 1
// ERROR_ARGS 2

typedef struct
{
    int N;
    double *Coordinates;
} DimensionVector;

typedef double (*NormaCalculator)(const DimensionVector *, const void *);

typedef struct
{
    int len;
    int capacity;
    DimensionVector **buffer;
} Vector;

DimensionVector *NewDimensionVector(int n, ...)
{
    va_list args;
    va_start(args, n);

    DimensionVector *result = malloc(sizeof(DimensionVector));
    if (!result)
    {
        va_end(args);
        return NULL;
    }
    double *coords = malloc(sizeof(double) * n);
    if (!coords)
    {
        free(result);
        va_end(args);
        return NULL;
    }

    result->Coordinates = coords;
    result->N = n;
    for (int i = 0; i < n; i++)
    {
        result->Coordinates[i] = va_arg(args, double);
    }
    va_end(args);

    return result;
}

void DestroyDimensionVector(DimensionVector *v)
{
    free(v->Coordinates);
    free(v);
}

DimensionVector *CopyDimensionVector(const DimensionVector *v)
{
    DimensionVector *result = malloc(sizeof(DimensionVector));
    if (!result)
    {
        return NULL;
    }

    double *coord = malloc(sizeof(double) * v->N);
    if (!coord)
    {
        free(result);
        return NULL;
    }
    result->N = v->N;
    result->Coordinates = coord;
    for (int i = 0; i < v->N; i++)
    {
        result->Coordinates[i] = v->Coordinates[i];
    }
    return result;
}

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
        DestroyDimensionVector(v->buffer[i]);
    }
    free(v->buffer);
    free(v);
}

int VectorPush(Vector *v, DimensionVector *value)
{
    if (v->len + 1 >= v->capacity)
    {
        v->capacity *= 2;
        DimensionVector **newBuffer = realloc(v->buffer, v->capacity * sizeof(DimensionVector *));
        if (!newBuffer)
        {
            return 1;
        }
        v->buffer = newBuffer;
    }
    v->buffer[v->len] = value;
    v->len++;
    return 0;
}

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
        DestroyVector(vectors[i]);
    }
    free(vectors);
}

Vector **GetMaxNorma(int *code, int dimension, int numberOfVectors, int numberOfNormas, ...)
{
    va_list args;
    va_start(args, numberOfNormas);

    if (dimension < 0 || numberOfVectors < 0 || numberOfNormas < 0 || numberOfNormas > 3)
    {
        *code = 2;
        va_end(args);
        return NULL;
    }

    DimensionVector **vectors = malloc(sizeof(DimensionVector *) * numberOfVectors);
    if (!vectors)
    {
        *code = 1;
        va_end(args);
        return NULL;
    }

    for (int i = 0; i < numberOfVectors; i++)
    {
        vectors[i] = va_arg(args, DimensionVector *);
        if (vectors[i] == NULL || vectors[i]->N != dimension)
        {
            *code = 2;
            for (int j = 0; j < i; j++)
            {
                DestroyDimensionVector(vectors[j]);
            }
            free(vectors);
            va_end(args);
            return NULL;
        }
    }

    Vector **output = malloc(sizeof(Vector *) * numberOfNormas);
    if (!output)
    {
        *code = 1;
        for (int i = 0; i < numberOfVectors; i++)
        {
            DestroyDimensionVector(vectors[i]);
        }
        free(vectors);
        va_end(args);
        return NULL;
    }

    for (int k = 0; k < numberOfNormas; k++)
    {
        output[k] = CreateVector(1);
        if (!output[k])
        {
            *code = 1;
            for (int i = 0; i < numberOfVectors; i++)
            {
                DestroyDimensionVector(vectors[i]);
            }
            free(vectors);
            FreeVectors(output, k);
            va_end(args);
            return NULL;
        }

        NormaCalculator f = va_arg(args, NormaCalculator);
        if (!f)
        {
            *code = 2;
            for (int i = 0; i < numberOfVectors; i++)
            {
                DestroyDimensionVector(vectors[i]);
            }
            free(vectors);
            FreeVectors(output, k + 1);
            va_end(args);
            return NULL;
        }

        void *arg = va_arg(args, void *);

        double longestNorma = -DBL_MAX;

        for (int i = 0; i < numberOfVectors; i++)
        {
            double norma = f(vectors[i], arg);
            if (norma > longestNorma)
            {
                longestNorma = norma;
                DestroyVector(output[k]);
                output[k] = CreateVector(1);
                if (!output[k] || VectorPush(output[k], CopyDimensionVector(vectors[i])))
                {
                    *code = 1;
                    for (int j = 0; j < numberOfVectors; j++)
                    {
                        DestroyDimensionVector(vectors[j]);
                    }
                    free(vectors);
                    FreeVectors(output, k + 1);
                    va_end(args);
                    return NULL;
                }
            }
            else if (fabs(longestNorma - norma) < EPS)
            {
                if (VectorPush(output[k], CopyDimensionVector(vectors[i])))
                {
                    *code = 1;
                    for (int j = 0; j < numberOfVectors; j++)
                    {
                        DestroyDimensionVector(vectors[j]);
                    }
                    free(vectors);
                    FreeVectors(output, k + 1);
                    va_end(args);
                    return NULL;
                }
            }
        }
    }

    for (int i = 0; i < numberOfVectors; i++)
    {
        DestroyDimensionVector(vectors[i]);
    }
    free(vectors);
    va_end(args);
    *code = 0;
    return output;
}

int main(void)
{
    int code = 0;
    int dimension = 3;
    int numberOfVectors = 3;
    int numberOfNormas = 3;
    int p = 1;
    double A[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    DimensionVector *v1 = NewDimensionVector(dimension, 1.0, 1.0, 1.0);
    DimensionVector *v2 = NewDimensionVector(dimension, 1.0, 1.0, 1.0);
    DimensionVector *v3 = NewDimensionVector(dimension, 1.0, 1.0, 1.0);

    if (!v1 || !v2 || !v3)
    {
        fprintf(stderr, "Memory allocation error for vectors.\n");
        code = 1;
    }
    else
    {
        Vector **result = GetMaxNorma(&code, dimension, numberOfVectors, numberOfNormas,
                                      v1, v2, v3,
                                      MaxNorma, NULL,
                                      SumNorma, &p,
                                      MatrixNorma, &A);

        if (code == 0)
        {
            for (int i = 0; i < numberOfNormas; i++)
            {
                printf("Longest vectors for norm %d:\n", i + 1);
                for (int j = 0; j < result[i]->len; j++)
                {
                    DimensionVector *v = result[i]->buffer[j];
                    for (int k = 0; k < v->N; k++)
                    {
                        printf("%lf ", v->Coordinates[k]);
                    }
                    printf("\n");
                }
                printf("\n");
            }
            FreeVectors(result, numberOfNormas);
        }
        else
        {
            fprintf(stderr, "Error in GetMaxNorma: %d\n", code);
        }
    }

    if (v1)
        DestroyDimensionVector(v1);
    if (v2)
        DestroyDimensionVector(v2);
    if (v3)
        DestroyDimensionVector(v3);

    return code;
}