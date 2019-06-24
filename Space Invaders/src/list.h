#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

struct list_node {
    struct list_node* next;
    struct list_node* previous;
    void* data;
};

struct list {
    struct list_node* head;
    int length;
};

struct list* create_list(void)
{
    struct list* l = (struct list*)malloc(sizeof(struct list));
    l->length = 0;

    return l;
}

void destroy_list(struct list* l)
{
    if (l->length > 0) {
        struct list_node* n = l->head;
        struct list_node* p;

        do {
            p = n;
            n = n->next;
            free(p);
        } while (n != l->head);
    }

    free(l);
}

void add_to_list(struct list* l, void* data)
{
    struct list_node* n = (struct list_node*)malloc(sizeof(struct list_node));
    n->data = data;

    if (l->length == 0) {
        l->head = n;
        n->next = n;
        n->previous = n;
    }
    else {
        n->next = l->head;
        n->previous = l->head->previous;
        n->previous->next = n;
        l->head->previous = n;
    }

    l->length++;
}

void remove_from_list(struct list* l, void* data)
{
    if (l->length > 0) {
        struct list_node* n = l->head;

        do {
            if (n->data == data) {
                n->next->previous = n->previous;
                n->previous->next = n->next;
                free(n);
            }

            n = n->next;
        } while (n != l->head);
    }
}

void iterate_list(struct list* l, void (*f)(void*))
{
    if (l->length > 0) {
        struct list_node* n = l->head;

        do {
            (*f)(n->data);
            n = n->next;
        } while (n != l->head);
    }
}

#endif
