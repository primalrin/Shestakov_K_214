#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_CHILDREN 100

typedef struct Node
{
    char value;                          // Значение узла
    struct Node *children[MAX_CHILDREN]; // Массив указателей на дочерние узлы
    int child_count;                     // Количество дочерних узлов
} Node;

Node *createNode(char value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL)
    {
        return NULL;
    }
    node->value = value;
    node->child_count = 0;
    memset(node->children, 0, sizeof(node->children));
    return node;
}

void freeTree(Node *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i < root->child_count; i++)
    {
        freeTree(root->children[i]);
    }
    free(root);
}

Node *parseExpression(const char **expr)
{
    while (**expr == ' ')
        (*expr)++;

    if (**expr == '\0' || **expr == ')' || **expr == ',')
        return NULL;

    Node *node = createNode(**expr);
    if (node == NULL)
    {
        return NULL;
    }
    (*expr)++;

    while (**expr == ' ')
        (*expr)++;

    if (**expr == '(')
    {
        (*expr)++;

        while (1)
        {
            Node *child = parseExpression(expr);
            if (child != NULL)
            {
                node->children[node->child_count++] = child;
            }

            while (**expr == ' ')
                (*expr)++;

            if (**expr == ')')
            {
                (*expr)++;
                break;
            }
            else if (**expr == ',')
            {
                (*expr)++;
            }
        }
    }

    return node;
}

int getTreeWidth(Node *root)
{
    if (root == NULL)
        return 0;

    int width = 1;
    for (int i = 0; i < root->child_count; i++)
    {
        width += getTreeWidth(root->children[i]);
    }
    return width;
}

void printTree(FILE *out, Node *root, int level, int *positions, int pos)
{
    if (root == NULL)
        return;

    positions[level] = pos;

    fprintf(out, "\n");
    for (int i = 0; i < level; i++)
    {
        for (int j = 0; j < positions[i]; j++)
        {
            fprintf(out, " ");
        }
        fprintf(out, "|");
    }
    for (int i = 0; i < pos; i++)
    {
        fprintf(out, " ");
    }
    fprintf(out, "%c", root->value);

    int totalWidth = getTreeWidth(root);
    int currentPos = 0;

    for (int i = 0; i < root->child_count; i++)
    {
        int childWidth = getTreeWidth(root->children[i]);
        printTree(out, root->children[i], level + 1, positions,
                  (totalWidth - currentPos - childWidth) / 2);
        currentPos += childWidth;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Cannot open input file: %s\n", argv[1]);
        return 1;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("Cannot open output file: %s\n", argv[2]);
        fclose(input);
        return 1;
    }

    char line[MAX_LINE];
    int tree_number = 1;
    int positions[MAX_LINE] = {0};

    while (fgets(line, sizeof(line), input))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        if (strlen(line) == 0)
            continue;

        fprintf(output, "\nTree #%d for expression: %s\n", tree_number++, line);

        const char *expr = line;
        Node *root = parseExpression(&expr);

        if (root != NULL)
        {
            printTree(output, root, 0, positions, 0);
            fprintf(output, "\n");
            freeTree(root);
        }
        else if (strlen(line) != 0)
        {
            fprintf(output, "Memory error\n");
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}