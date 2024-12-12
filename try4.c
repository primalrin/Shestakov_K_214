#include <stdio.h>
#include <stdlib.h>

// Определение типа tvalue (может быть любым типом данных)
typedef int tvalue;

// Структура для элемента очереди
typedef struct queue_item
{
    tvalue value;
    struct queue_item *next;
} queue_item;

// Структура для очереди
typedef struct queue
{
    queue_item *head;
    queue_item *tail;
} queue;

// Функция для создания нового элемента очереди
queue_item *create_queue_item(tvalue value)
{
    queue_item *new_item = (queue_item *)malloc(sizeof(queue_item));
    if (new_item == NULL)
    {
        return NULL; // Ошибка выделения памяти
    }
    new_item->value = value;
    new_item->next = NULL;
    return new_item;
}

// Функция для добавления элемента в очередь
int enqueue(queue *q, tvalue value)
{
    queue_item *new_item = create_queue_item(value);
    if (new_item == NULL)
    {
        return -1; // Ошибка выделения памяти
    }

    if (q->head == NULL)
    {
        q->head = new_item;
        q->tail = new_item;
    }
    else
    {
        q->tail->next = new_item;
        q->tail = new_item;
    }
    return 0; // Успешное добавление
}

// Функция для создания подочереди
int subqueue(queue *initial, size_t without, queue **result)
{
    if (initial == NULL || result == NULL)
    {
        return -1; // Некорректные входные параметры
    }

    *result = (queue *)malloc(sizeof(queue));
    if (*result == NULL)
    {
        return -2; // Ошибка выделения памяти для новой очереди
    }
    (*result)->head = NULL;
    (*result)->tail = NULL;

    if (initial->head == NULL)
    {
        return 0; // Исходная очередь пуста
    }

    // Подсчет количества элементов в исходной очереди
    size_t count = 0;
    queue_item *current = initial->head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    // Проверка, что without не превышает количество элементов в очереди
    if (without >= count)
    {
        free(*result); // Освобождаем память, выделенную для новой очереди
        *result = NULL;
        return -3; // without больше или равно количеству элементов
    }

    // Вычисление количества элементов для подочереди
    size_t subqueue_size = count - without;

    current = initial->head;
    for (size_t i = 0; i < subqueue_size; i++)
    {
        if (enqueue(*result, current->value) != 0)
        {
            // Очистка уже созданной части подочереди при ошибке выделения памяти
            while ((*result)->head != NULL)
            {
                queue_item *temp = (*result)->head;
                (*result)->head = (*result)->head->next;
                free(temp);
            }
            free(*result);
            *result = NULL;
            return -2;
        }
        current = current->next;
    }

    return 0; // Успешное создание подочереди
}

// Функция для печати содержимого очереди
void print_queue(queue *q)
{
    if (q == NULL)
    {
        printf("Queue is NULL\n");
        return;
    }

    if (q->head == NULL)
    {
        printf("Queue is empty\n");
        return;
    }

    queue_item *current = q->head;
    printf("Queue: ");
    while (current != NULL)
    {
        printf("%d ", current->value);
        current = current->next;
    }
    printf("\n");
}

// Функция для очистки очереди
void clear_queue(queue *q)
{
    if (q == NULL)
    {
        return;
    }

    queue_item *current = q->head;
    while (current != NULL)
    {
        queue_item *temp = current;
        current = current->next;
        free(temp);
    }
    q->head = NULL;
    q->tail = NULL;
}

int main()
{
    queue initial_queue;
    initial_queue.head = NULL;
    initial_queue.tail = NULL;

    // Добавление элементов в исходную очередь
    for (int i = 1; i <= 7; i++)
    {
        if (enqueue(&initial_queue, i) != 0)
        {
            printf("Failed to enqueue element %d\n", i);
            clear_queue(&initial_queue);
            return 1;
        }
    }

    printf("Initial queue: ");
    print_queue(&initial_queue);

    queue *result_queue = NULL;
    size_t without = 2;

    int status = subqueue(&initial_queue, without, &result_queue);

    if (status == 0)
    {
        printf("Subqueue without %zu last elements: ", without);
        print_queue(result_queue);
    }
    else
    {
        printf("Error creating subqueue: %d\n", status);
    }

    // Освобождение памяти
    clear_queue(&initial_queue);
    clear_queue(result_queue);
    free(result_queue); // Освобождаем память самой структуры очереди

    return 0;
}