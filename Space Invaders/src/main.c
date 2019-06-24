/*
 ============================================================================
 Name        : Space Invaders
 Author      : D.Nest
 Version     : 1.0
 Description :
               пробел - выстрел
               ESC - выход
               r - увеличить количество красного цвета
               g - увеличить количество зелёного цвета
               b - увеличить количество синего цвета
               list.h - хранение информации обо всех объектах на карте
               object.h - управление движением и состоянием объектов
               vector.h - рассчёт позиции
               main.h - основная информация и анимации
 ============================================================================
 */

#include <stdio.h>
#include <time.h>
#if defined(__MINGW32__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "vector.h"
#include "list.h"
#include "object.h"
#include "main.h"
FILE* save_file; // Файл настроек

SDL_Window* window; // Окно
SDL_Renderer* renderer;// Рендерер
SDL_Texture* texture;//Текстуы
SDL_Event event;//События
uint8_t red = 0xFF, green = 0xFF, blue = 0xFF; //Переменные цвета
unsigned int ur, ug, ub; // Переменные для считывания код цвета фона из файла
int initialize(void)
{
    // Считываем настройки игры из файла
    save_file = fopen("settings.txt", "r");
    // Считываем цвет фона
    fscanf(save_file, "%X %X %X", &ur, &ug, &ub);
    red = (uint8_t)ur;
    green = (uint8_t)ug;
    blue = (uint8_t)ub;
    fclose(save_file);
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // Если SDL не запустился
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Space Invaders", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN); //Создаем окно
    if (window == NULL) {//Если окно не создалось
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);//Подключаем рендерер
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    SDL_Surface* surface = SDL_LoadBMP("resources/sprite_sheet.bmp"); // подключаем спрайты
    if (surface == NULL) {
        fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 4;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);//Создание текстур
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 5;
    }
    objects = create_list(); //Создание списка объектов
    player = create_object(create_vector(WINDOW_WIDTH / 2 - 13, WINDOW_HEIGHT - 38), PLAYER); //Создаем игрока
    add_to_list(objects, player);//Добавляем его
    player_laser = create_object(create_vector(0, 0), LASER);//Лазер игрока
    add_to_list(objects, player_laser);

    time_t now = time(0);

    int i = 0;
    for (int c = 1; c <= 11; c++) { // Создаем пришельцев
        for (int r = 1; r <= 5; r++) {
            int x = (WINDOW_WIDTH / 13) * c - 16;
            int y = (WINDOW_HEIGHT / 15) * r + 70;

            if (r == 1)
                invaders[i] = create_object(create_vector(x, y), INVADER1);
            else if (r < 4)
                invaders[i] = create_object(create_vector(x, y), INVADER2);
            else
                invaders[i] = create_object(create_vector(x, y), INVADER3);

            invaders[i]->animation = now + 1 * (5 - r);

            add_to_list(objects, invaders[i++]);
        }
    }

    SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF); //Выставляем цвет фона

    return 0;
}

void handle_input(SDL_KeyboardEvent* key)//Управление
{
    if (key->type == SDL_KEYUP)
        player->velocity->x = 0;
    else
        switch (key->keysym.scancode) {
        case 0x50: // Левая стрелочка
            if (player->position->x > 0)
                player->velocity->x = -1;
            break;

        case 0x4F: // Правая стрелочка
            if (player->position->x < WINDOW_WIDTH - 26)
                player->velocity->x = 1;
            break;

        case 0x2C: // Выстрел
            if (player_laser->state == DEAD) {
                player_laser->position->x = player->position->x + 10;
                player_laser->position->y = player->position->y - 16;
                player_laser->state = ALIVE;
            }

            break;

        default:
            break;
        }
}

void draw_object(void* v) // Смена анимаций объектов
{
    struct object* o = (struct object*)v;

    if (o->state == DEAD)
        return;

    SDL_Rect destination;
    destination.x = o->position->x;
    destination.y = o->position->y;

    SDL_Rect clip;
    switch (o->type) {//Смена анимации
    case PLAYER:
        clip = PLAYER_CLIP;
        break;

    case INVADER1://Первый пришелец
        if (o->state != DYING) {
            clip = INVADER1_CLIP;
            destination.x += 4;
            if (o->frame == 1)
                clip.x += 16;
        }
        else
            clip = DYING_INVADER_CLIP;
        break;

    case INVADER2://Второй пришелец
        if (o->state != DYING) {
            clip = INVADER2_CLIP;
            destination.x += 1;
            if (o->frame == 1)
                clip.x += 22;
        }
        else
            clip = DYING_INVADER_CLIP;
        break;

    case INVADER3://Третий пришелец
        if (o->state != DYING) {
            clip = INVADER3_CLIP;
            if (o->frame == 1)
                clip.x += 24;
        }
        else
            clip = DYING_INVADER_CLIP;
        break;

    case LASER:
        clip = LASER_CLIP;
        break;

    default:
        break;
    }

    destination.w = clip.w;
    destination.h = clip.h;

    SDL_RenderCopy(renderer, texture, &clip, &destination);
}

int main(int argc, char** argv)
{

    int quit = initialize();

    if (quit != 0)
        return quit;

    while (quit == 0) {//Пока игра активна(проверка на клавиши)
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                printf("quit!");
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handle_input(&event.key);
                break;
            default:
                break;
            }
            switch (event.key.keysym.sym) {
            case SDLK_r://Увеличение красного цвета
                save_file = fopen("settings.txt", "w");
                fprintf(save_file, "%X %X %X\n", red, green, blue);
                fclose(save_file);
                red = red + 10;
                SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
                break;
            case SDLK_g://Увелечение зеленого цвета
                save_file = fopen("settings.txt", "w");
                fprintf(save_file, "%X %X %X\n", red, green, blue);
                fclose(save_file);
                green = green + 10;
                SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
                break;
            case SDLK_b://Увелечение синего цвета
                save_file = fopen("settings.txt", "w");
                fprintf(save_file, "%X %X %X\n", red, green, blue);
                fclose(save_file);
                blue = blue - 10;
                SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
                break;
            case SDLK_ESCAPE: // Выход из игры
                // Нажата клавиша ESC, меняем флаг выхода
                quit = 1;
                printf("quit!");

                break;
            }
        }

        SDL_RenderClear(renderer);

        //Обновляем списки
        iterate_list(objects, &update_object);
        iterate_list(objects, &draw_object);

        SDL_RenderPresent(renderer);
    }
    //Уничтожаем все объекты
    destroy_list(objects);
    destroy_object(player);
    destroy_object(player_laser);

    for (int i = 0; i < 55; i++)
        destroy_object(invaders[i]);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
