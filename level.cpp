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
    ball_speed = 5;
    ball_angle = 90;
    hit_cooldown = 0;
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
    ball_speed = 5;
    ball_angle = angle;
    grid_x = x;
    grid_y = y;
    hit_cooldown = 0;
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

void Level::set_ball_angle(int angle)
{
    ball_angle = angle;
}


bool Level::is_near(int number)
{
    for (int i = 0; i < map.size(); i++)
    {
        if (i != number)
        {
            if (abs(ball_x - (map[i].get_coord().x() + brick_size_x/2)) < brick_size_x &&
                abs(ball_y - (map[i].get_coord().y() + brick_size_y/2)) < brick_size_y)
            {
                return true;
            }
        }
    }
    return false;
}


int Level::update(int width, int height)
{
    double PI = 3.14159265;

    if (board_y < height/4*3)
        board_y = height/4*3;

    if (ball_y > height/2 + 50) // торможение для отладки, потом убрать
        ;//ball_speed = 10;
    else ball_speed = 5;

    if (ball_angle < 0) // приведение угла для подсчета
        ball_angle = 360 + ball_angle;
    ball_angle = (int)ball_angle % 360;



    if (ball_x <= 0) // левая стенка
    {
        ball_angle = 180 - ball_angle + (rand()%3); // отражение + погрешность
        ball_x = 1;
    }
    else if (ball_x >= width) // правая стенка
    {
        ball_angle = 180 - ball_angle + (rand()%3 - 2);// отражение + погрешность
        ball_x = width - 1;
    }
    else if (ball_y <= 0) //верхняя стенка
    {
        ball_angle = -ball_angle + (rand()%3 - 1);// отражение + погрешность
        ball_y = 1;
    }
    else if (ball_y >= height) // проигрыш (нижняя стенка)
    {
        return -1;
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
            /*ball_x -= ball_speed * cos(ball_angle * PI / 180.0); // возврат позиции
            ball_y -= ball_speed * sin(ball_angle * PI / 180.0);*/
            if (brick_size_x/2 - abs(dx) < brick_size_y/2 - abs(dy)) // слева/справа
            {   
                if (abs(dy) > brick_size_y/2 - 5 && !is_near(i)) // если попало на угол
                {                                                // и рядом нет блока
                    if (ball_angle >= 270 || ball_angle <= 90) // движется вправо
                        ball_angle += dy; // сбитое отражение
                    else if (ball_angle > 90 && ball_angle < 270) // движется влево
                        ball_angle -= dy; // сбитое отражение
                }
                ball_angle = 180 - ball_angle; // отражение
                map.takeAt(i); //  удаление блока
                break;
            }
            else if (brick_size_x/2 - abs(dx) >= brick_size_y/2 - abs(dy)) // снизу/сверху
            {
                if (abs(dx) > brick_size_x/2 - 5 && !is_near(i)) // если попало на край
                {                                                // и рядом нет блока
                    if (ball_angle >= 0 && ball_angle <= 180) // движется вверх
                        ball_angle += dx; // сбитое отражение
                    else if (ball_angle > 180 && ball_angle < 360) // движется вниз
                        ball_angle -= dx; // сбитое отражение
                }
                ball_angle = -ball_angle; // отражение
                map.takeAt(i); //  удаление блока
                break;
            }
        }
    }

    ball_x += ball_speed * cos(ball_angle * PI / 180.0); // новые координаты
    ball_y += ball_speed * sin(ball_angle * PI / 180.0);


    // столкновение с доской
    dx = ball_x - (board_x + board_width/2); // растояние от шарика до центра доски
    dy = ball_y - (board_y + board_height/2);
    if (hit_cooldown == 0) // если отражение возможно
    {
        if (abs(dx) < board_width/2 && abs(dy) < board_height/2) // столкновение
        {
            ball_x -= ball_speed * cos(ball_angle * PI / 180.0); // возврат позиции
            ball_y -= ball_speed * sin(ball_angle * PI / 180.0); // шарика
            ball_angle = -ball_angle; // отражение
            ball_angle += dx/(board_width/100); // доп угол отражения
            hit_cooldown = 100; // задержка перед следующим отражением
        }
    }
    hit_cooldown -= ball_speed;
    if (hit_cooldown < 0)
        hit_cooldown = 0;


    return 0;
}


void Level::load_map()
{
    map.clear(); // очистка карты от остатков

    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
        {   // тестовая карта, ЗАМЕНИТЬ потом
            Brick *brick = new Brick(QPoint(i*brick_size_x, j*brick_size_y),
                                    QColor(i*100%255, j*100%255, (i+j)*100%255));
            map.push_front(*brick);
            delete brick;
        }

}
