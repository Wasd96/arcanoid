#include "level.h"

Level::Level()
{
    brick_size_x = 5;
    brick_size_y = 5;
    grid_x = 5;
    grid_y = 5;
    board_x = 300;
    board_y = 500;
    board_height = 20;
    board_width  = 100;
    ball_x = 300;
    ball_y = 800;
    ball_speed = 1;
    ball_angle = 90;
    map.append(Brick(QPoint(0,0),QColor(0,0,0))); // от бага, ФИКС нужен нормальный
}

Level::Level(short x, short y, double angle)
{
    brick_size_x = 5;
    brick_size_y = 5;
    board_x = 300;
    board_y = 500;
    board_height = 20;
    board_width  = 100;
    ball_x = 300;
    ball_y = 800;
    ball_speed = 1;
    ball_angle = angle;
    grid_x = x;
    grid_y = y;
    map.append(Brick(QPoint(0,0),QColor(0,0,0)));
}

void Level::set_brick_size(int w, int h)
{
    brick_size_x = w / grid_x;
    brick_size_y = h/2 / grid_y; // (h/2) - чтобы не занимало все пространсвто
}

void Level::set_grid(int x, int y)
{
    grid_x = x;
    grid_y = y;
}

void Level::set_board_coord(int x, int y)
{
    board_x = x;
    board_y = y;
}

void Level::set_ball_coord(double x, double y)
{
    ball_x = x;
    ball_y = y;
}

int Level::ball_update(int width, int height)
{
    double PI = 3.14159265;

    if (ball_y > height/2 + 50) // торможение для отладки, потом убрать
        ball_speed = 10;
    else ball_speed = 1;

    if (ball_angle < 0) // приведение угла для подсчета
        ball_angle = 360 + ball_angle;
    ball_angle = (int)ball_angle % 360;



    if (ball_x <= 0) // левая стенка
    {
        ball_angle = 180 - ball_angle + (rand()%3 - 1); // отражение + погрешность
        ball_x = 1;
    }
    else if (ball_x >= width) // правая стенка
    {
        ball_angle = 180 - ball_angle + (rand()%3 - 1);// отражение + погрешность
        ball_x = width - 1;
    }
    if (ball_y <= 0) //верхняя стенка
    {
        ball_angle = -ball_angle + (rand()%3 - 1);// отражение + погрешность
        ball_y = 1;
    }
    else if (ball_y >= height) // проигрыш (нижняя стенка)
    {
        // return -1;
        ball_angle = -ball_angle  + (rand()%3 - 1);
        ball_y = height - 1;
    }

    // столкновения с кубиками
    double dx = 0; // начальные значения разницы координат
    double dy = 0;
    for (int i = 0; i < map.size(); i++)
    {
        dx = ball_x - (map[i].get_coord().x()+brick_size_x/2); // разница координат
        dy = ball_y - (map[i].get_coord().y()+brick_size_y/2); // шарика и кирпичика
        if (abs(dx) <= brick_size_x/2 && abs(dy) <= brick_size_y/2) // столкновение
        {
            ball_x -= ball_speed * cos(ball_angle * PI / 180.0); // возврат позиции
            ball_y -= ball_speed * sin(ball_angle * PI / 180.0);
            if (brick_size_x/2 - abs(dx) < brick_size_y/2 - abs(dy)) // слева/справа
            {   
                if (brick_size_y/2 - abs(dy) < 8) // если попало на край
                {
                    if (ball_angle >= 270 || ball_angle <= 90) // движется вправо
                    {
                        if (dy > 0)
                            ball_angle += 5*(brick_size_x/2 - abs(dx)); // кривое отражение
                        else
                            ball_angle -= 5*(brick_size_x/2 - abs(dx));
                    }
                    else if (ball_angle > 90 && ball_angle < 270) // движется влево
                    {
                        if (dy > 0)
                            ball_angle -= 5*(brick_size_x/2 - abs(dx)); // кривое отражение
                        else
                            ball_angle += 5*(brick_size_x/2 - abs(dx));
                    }
                }
                ball_angle = 180 - ball_angle; // отражение
                if (dy > 0) // выведение из прямых углов
                    ball_angle--;
                else
                    ball_angle++;
                map.takeAt(i); //  удаление блока
                break;
            }
            else if (brick_size_x/2 - abs(dx) >= brick_size_y/2 - abs(dy)) // снизу/сверху
            {
                if (brick_size_x/2 - abs(dx) < 8) // если попало на край
                {
                    if (ball_angle >= 0 && ball_angle <= 180) // движется вверх
                    {
                        if (dx > 0)
                            ball_angle -= 5*(brick_size_x/2 - abs(dx)); // кривое отражение
                        else
                            ball_angle += 5*(brick_size_x/2 - abs(dx));
                    }
                    else if (ball_angle > 180 && ball_angle < 360) // движется вниз
                    {
                        if (dx > 0)
                            ball_angle += 5*(brick_size_x/2 - abs(dx)); // кривое отражение
                        else
                            ball_angle -= 5*(brick_size_x/2 - abs(dx));
                    }
                }
                ball_angle = -ball_angle; // отражение
                if (dx > 0) // выведение из прямых углов
                    ball_angle++;
                else
                    ball_angle--;
                map.takeAt(i); //  удаление блока
                break;
            }

        }

    }

    ball_x += ball_speed * cos(ball_angle * PI / 180.0); // новые координаты
    ball_y += ball_speed * sin(ball_angle * PI / 180.0);

}


void Level::load_map()
{
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
        {   // тестовая карта, ЗАМЕНИТЬ потом
            Brick *brick = new Brick(QPoint(i*brick_size_x, j*brick_size_y),
                                    QColor(i*100%255, j*100%255, (i+j)*100%255));
            map.push_front(*brick);
            delete brick;
        }

}
