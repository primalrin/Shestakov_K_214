#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Структура для представления матрицы
typedef struct
{
    int **data;
    int rows;
    int cols;
} Matrix;

// Функция для создания матрицы заданного размера
Matrix createMatrix(int rows, int cols)
{
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        matrix.data[i] = (int *)calloc(cols, sizeof(int)); // Инициализируем нулями
    }
    return matrix;
}

// Функция для освобождения памяти, выделенной под матрицу
void freeMatrix(Matrix matrix)
{
    for (int i = 0; i < matrix.rows; i++)
    {
        free(matrix.data[i]);
    }
    free(matrix.data);
}

// Функция для чтения матрицы из файла
Matrix readMatrixFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }

    Matrix matrix;
    matrix.rows = 0;
    matrix.cols = 0;
    matrix.data = NULL;

    char line[1024]; // Предполагаем максимальную длину строки

    while (fgets(line, sizeof(line), file) != NULL)
    {
        matrix.rows++;
        int *row_data = (int *)malloc(1 * sizeof(int));
        int row_cols = 0;
        char *token = strtok(line, " \t\n");
        while (token != NULL)
        {
            row_cols++;
            row_data = (int *)realloc(row_data, row_cols * sizeof(int));
            row_data[row_cols - 1] = atoi(token);
            token = strtok(NULL, " \t\n");
        }
        if (row_cols > matrix.cols)
        {
            matrix.cols = row_cols;
        }

        matrix.data = (int **)realloc(matrix.data, matrix.rows * sizeof(int *));
        matrix.data[matrix.rows - 1] = (int *)calloc(matrix.cols, sizeof(int));
        for (int i = 0; i < row_cols; i++)
        {
            matrix.data[matrix.rows - 1][i] = row_data[i];
        }
        free(row_data);
    }

    fclose(file);
    return matrix;
}

// Функция для вывода матрицы в файл
void writeMatrixToFile(const char *filename, Matrix matrix)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Ошибка открытия файла для записи");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < matrix.rows; i++)
    {
        for (int j = 0; j < matrix.cols; j++)
        {
            fprintf(file, "%d ", matrix.data[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Функция для поиска седловых точек
void findSaddlePoints(Matrix matrix, const char *outputFilename)
{
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL)
    {
        perror("Ошибка открытия выходного файла");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < matrix.rows; i++)
    {
        for (int j = 0; j < matrix.cols; j++)
        {
            int currentElement = matrix.data[i][j];

            // Проверка на минимум в строке
            int minInRow = currentElement;
            for (int k = 0; k < matrix.cols; k++)
            {
                if (matrix.data[i][k] < minInRow)
                {
                    minInRow = matrix.data[i][k];
                }
            }

            // Проверка на максимум в столбце
            int maxInCol = currentElement;
            for (int k = 0; k < matrix.rows; k++)
            {
                if (matrix.data[k][j] > maxInCol)
                {
                    maxInCol = matrix.data[k][j];
                }
            }

            // Если элемент минимален в строке и максимален в столбце, это седловая точка
            if (currentElement == minInRow && currentElement == maxInCol)
            {
                fprintf(outputFile, "Строка: %d, Столбец: %d\n", i, j);
            }
        }
    }

    fclose(outputFile);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Использование: %s <входной_файл> <выходной_файл>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *inputFilename = argv[1];
    const char *outputFilename = argv[2];

    Matrix matrix = readMatrixFromFile(inputFilename);

    printf("Размеры матрицы: строк %d, столбцов %d\n", matrix.rows, matrix.cols);

    findSaddlePoints(matrix, outputFilename);

    freeMatrix(matrix);

    return EXIT_SUCCESS;
}