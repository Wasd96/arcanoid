#include "core.h"

Core::Core()
{
    current_level = 0;
    level = new Level(5,5); // тестовое значение
    level->set_brick_size(5,5);
}


void Core::start_game(int w, int h)
{
    level->set_grid(5,5); // создание сетки
    level->set_brick_size(w, h); // задание размеров блока

    level->load_map(current_level); // загрузка текущего уровня

}
