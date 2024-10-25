#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

// Function to calculate binomial coefficients
double binomial_coef(int n, int k)
{
    if (k > n)
        return 0;
    if (k == 0 || k == n)
        return 1;

    double result = 1;
    for (int i = 0; i < k; i++)
    {
        result *= (n - i);
        result /= (i + 1);
    }
    return result;
}

// Function to calculate power
double power(double base, int exp)
{
    double result = 1.0;
    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

// Main function for polynomial recomposition
int recompose_polynomial(double epsilon, double a, double **result, int degree, ...)
{
    if (degree < 0 || result == NULL || epsilon <= 0)
    {
        return -1;
    }

    // Allocate memory for the resulting coefficients
    *result = (double *)calloc(degree + 1, sizeof(double));
    if (*result == NULL)
    {
        return -1;
    }

    // Get the coefficients of the original polynomial
    va_list args;
    va_start(args, degree);
    double *coeffs = (double *)calloc(degree + 1, sizeof(double));
    if (coeffs == NULL)
    {
        free(*result);
        *result = NULL;
        va_end(args);
        return -1;
    }

    for (int i = 0; i <= degree; i++)
    {
        coeffs[i] = va_arg(args, double);
    }
    va_end(args);

    // Calculate new coefficients
    for (int i = 0; i <= degree; i++)
    {
        for (int j = i; j <= degree; j++)
        {
            double term = coeffs[j] * binomial_coef(j, i) * power(-a, j - i);
            (*result)[i] += term;

            // Check for accuracy
            if (fabs(term) < epsilon && j < degree)
            {
                continue;
            }
        }
    }

    free(coeffs);
    return 0;
}

// Function to check the correctness of the result
double evaluate_polynomial(double *coeffs, int degree, double x)
{
    double result = 0.0;
    for (int i = 0; i <= degree; i++)
    {
        result += coeffs[i] * power(x, i);
    }
    return result;
}

int main()
{
    double *result = NULL;
    double epsilon = 1e-10;
    double a = 2.0;
    int degree = 2;

    // Example: f(x) = 1 + 2x + x^2
    if (epsilon <= 0 || degree < 0)
    {
        fprintf(stderr, "Invalid input parameters.\n");
        return -1;
    }

    int status = recompose_polynomial(epsilon, a, &result, degree, 1.0, 2.0, 1.0);

    if (status == 0 && result != NULL)
    {
        printf("Original polynomial: f(x) = 1 + 2x + x^2\n");
        printf("Recomposition by powers of (x - %.1f):\n", a);

        for (int i = 0; i <= degree; i++)
        {
            printf("g_%d = %f\n", i, result[i]);
        }

        // Check correctness for several points
        double test_points[] = {0.0, 1.0, 2.0, 3.0};
        int num_points = sizeof(test_points) / sizeof(test_points[0]);

        printf("\nVerification of the recomposition correctness:\n");
        for (int i = 0; i < num_points; i++)
        {
            double x = test_points[i];
            double original = 1.0 + 2.0 * x + x * x;

            // Calculate the value of the recomposed polynomial
            double recomposed = 0.0;
            for (int j = 0; j <= degree; j++)
            {
                recomposed += result[j] * power(x - a, j);
            }

            printf("x = %.1f: original = %.10f, recomposed = %.10f, diff = %.10e\n",
                   x, original, recomposed, fabs(original - recomposed));
        }

        free(result);
    }
    else
    {
        fprintf(stderr, "An error occurred during the recomposition.\n");
        return -1;
    }

    return 0;
}