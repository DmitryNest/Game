#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <time.h>
#include "vector.h"
#include "list.h"
#include "main.h"

enum object_type {
    PLAYER,
    INVADER1,
    INVADER2,
    INVADER3,
    MYSTERY_SHIP,
    LASER
};

enum object_state {
    ALIVE,
    DYING,
    DEAD
};

struct object {
    struct vector* position;
    struct vector* velocity;
    enum object_type type;
    enum object_state state;
    time_t animation;
    time_t reverse_direction;
    int direction;
    int frame;
};

struct object* create_object(struct vector* position, enum object_type type)//Создание объекта
{
    struct object* o = (struct object*)malloc(sizeof(struct object));
    o->position = position;
    o->type = type;
    o->frame = 0;
    o->animation = time(0);
    o->reverse_direction = o->animation;
    o->direction = 0;

    if (type == LASER) {
        o->velocity = create_vector(0, -1);
        o->state = DEAD;
    }
    else {
        o->velocity = create_vector(0, 0);
        o->state = ALIVE;
    }

    return o;
}

void destroy_object(struct object* o)//Уничтожение оъекта
{
    destroy_vector(o->position);
    destroy_vector(o->velocity);

    free(o);
}

void check_collision(struct vector* collision, struct object* a, struct object* b)//Проверка коллизии
{

    if (a->position->x >= b->position->x && a->position->x <= b->position->x + 16 && a->position->y >= b->position->y && a->position->y <= b->position->y + 16) {
        collision->x = 1;
        collision->y = 1;
    }
}

void update_object(void* v)//Обновление позиции объекта
{
    struct object* o = (struct object*)v;
    time_t now = time(0);

    if (o->state == DEAD)
        return;

    if (o->type == LASER) {
        if (o->position->y < -12 || o->position->y > WINDOW_HEIGHT + 13)
            o->state = DEAD;

        struct vector* collision = create_vector(0, 0);
        struct list_node* n = objects->head;
        do {
            struct object* c = (struct object*)n->data;

            if (c->state == ALIVE && c != o) {
                check_collision(collision, o, c);

                if (collision->x != 0 || collision->y != 0) {
                    o->state = DEAD;
                    c->state = DYING;
                    break;
                }
            }

            n = n->next;
        } while (n != objects->head);

        destroy_vector(collision);
    }

    if (o->type == INVADER1 || o->type == INVADER2 || o->type == INVADER3) {
        if (now - o->animation > 5) { // через каждые 5 проигранных анимаций
            if (o->state != DYING) {
                if (now - o->reverse_direction > 30) { // если пришельцы не достигли края
                    o->reverse_direction = now;
                    o->animation = now;
                    o->direction = (o->direction > 0) ? -1 : 1;
                    o->position->y += 20; // спускаем их на 20 пикселей
                }
                else {//иначе
                    o->position->x = (o->direction > 0) ? o->position->x - 10 : o->position->x + 10;// передвигаем на 10 пикселей в зависимости от направления
                    o->animation = now;
                }
            }
            else
                o->state = DEAD;
        }
    }

    if (o->type == PLAYER || o->type == MYSTERY_SHIP) {
        o->position->x += o->velocity->x;
        o->position->y += o->velocity->y;
    }
    if(o->type == LASER){
        o->position->x += o->velocity->x;
        o->position->y += o->velocity->y*10;
    }
}

#endif
