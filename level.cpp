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
    hit_cooldown = 0;
    ball_last_x = ball_x;
    ball_last_y = ball_y;
    //map.append(Brick(QPoint(0,0),QColor(0,0,0))); // от бага, ФИКС нужен нормальный
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
    ball_speed = 1.5;
    ball_angle = angle;
    grid_x = x;
    grid_y = y;
    hit_cooldown = 0;
    ball_last_x = ball_x;
    ball_last_y = ball_y;
    //map.append(Brick(QPoint(0,0),QColor(0,0,0)));
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

void Level::set_ball_speed(double speed)
{
    ball_speed = speed;
}


int Level::update(int width, int height)
{
    double PI = 3.14159265;

    if (board_y < height/4*3)
        board_y = height/4*3;

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


    ball_last_x = ball_x; // старые координаты
    ball_last_y = ball_y;
    ball_x -= ball_speed * cos(ball_angle * PI / 180.0); // новые координаты
    ball_y -= ball_speed * sin(ball_angle * PI / 180.0);

    // столкновения с кубиками
    double k = 0; // коэффициент для ур-ния прямой
    double b = 0; // коэффициент для ур-ния прямой
    double cross_y_left = 0;    // точка столкновения с левой гранью
    double cross_y_right = 0;   // точка столкновения с правой гранью
    double cross_x_bottom = 0;  // точка столкновения с нижней гранью
    double cross_x_top = 0;     // точка столкновения с верхней гранью

    if ((int)ball_angle%90 != 0) // если это не прямой угол
        k = tan(ball_angle * PI / 180.0);
    else k = 0;
    int hit = 0; // флаг столкновения

    for (int i = 0; i < map.size(); i++)
    {
        int brick_x = map[i].get_coord().x(); // координаты рассматриваемого кирпичика
        int brick_y = map[i].get_coord().y();

        if (ball_x >= brick_x
                && ball_x <= brick_x + brick_size_x
                && ball_y >= brick_y
                && ball_y <= brick_y + brick_size_y) // если шарик внутри крипичика
        {
            b = ball_last_y - ((ball_y-ball_last_y)/(ball_x-ball_last_x))*ball_last_x;

            cross_x_bottom = ((brick_y + brick_size_y) - b)/k; // точки пересечений с гранями
            cross_x_top = (brick_y - b)/k;
            cross_y_right = k*(brick_x + brick_size_x - ball_x) + ball_y;
            cross_y_left = k*(brick_x - ball_x) + ball_y;

            if ((int)ball_angle%90 != 0)
            {
                if (cross_x_bottom <= brick_x + brick_size_x
                        && cross_x_bottom >= brick_x) // низ
                {
                    if (ball_angle > 0 && ball_angle < 90) // справа снизу
                        if (cross_x_bottom <= ball_last_x && cross_x_bottom >= ball_x)
                            hit = 2;
                    if (ball_angle > 90 && ball_angle < 180) // слева снизу
                        if (cross_x_bottom >= ball_last_x && cross_x_bottom <= ball_x)
                            hit = 2;
                }

                if (cross_y_left <= brick_y + brick_size_y
                        && cross_y_left >= brick_y) // лево
                {
                    if (ball_angle > 90 && ball_angle < 180) // слева снизу
                        if (cross_y_left <= ball_last_y && cross_y_left >= ball_y)
                            hit = 1;
                    if (ball_angle > 180 && ball_angle < 270) // слева сверху
                        if (cross_y_left >= ball_last_y && cross_y_left <= ball_y)
                            hit = 1;
                }

                if (cross_y_right <= brick_y + brick_size_y
                        && cross_y_right >= brick_y) // право
                {
                    if (ball_angle > 0 && ball_angle < 90) // справа снизу
                        if (cross_y_right <= ball_last_y && cross_y_right >= ball_y)
                            hit = 1;
                    if (ball_angle > 270 && ball_angle < 360) // справа сверху
                        if (cross_y_right >= ball_last_y && cross_y_right <= ball_y)
                            hit = 1;
                }
                if (cross_x_top <= brick_x + brick_size_x
                        && cross_x_top >= brick_x) // верх
                {
                    if (ball_angle > 270 && ball_angle < 360) // справа сверху
                        if (cross_x_top <= ball_last_x && cross_x_top >= ball_x)
                            hit = 2;
                    if (ball_angle > 180 && ball_angle < 270) // слева сверху
                        if (cross_x_top >= ball_last_x && cross_x_top <= ball_x)
                            hit = 2;
                }


            }

            else
            {
                if (ball_angle == 0 || ball_angle == 180)
                    hit = 1;
                else if (ball_angle == 90 || ball_angle == 270)
                    hit = 2;
            }

            if (hit == 1) // лево/право
            {
                ball_angle = 360 - (180 + ball_angle);
                ball_x = ball_last_x;
                ball_y = ball_last_y;
                map.takeAt(i);
            }
            else if (hit == 2) // верх/низ
            {
                ball_angle = -ball_angle;
                ball_x = ball_last_x;
                ball_y = ball_last_y;
                map.takeAt(i);
            }

        }
    }


    // столкновение с доской
    double dx = ball_x - (board_x + board_width/2); // растояние от шарика до центра доски
    double dy = ball_y - (board_y + board_height/2);
    if (hit_cooldown == 0) // если отражение возможно
    {
        if (abs(dx) < board_width/2 && abs(dy) < board_height/2) // столкновение
        {
            ball_x = ball_last_x; // возврат позиции
            ball_y = ball_last_y; // шарика
            ball_angle = -ball_angle; // отражение
            ball_angle += dx/(board_width/100); // доп угол отражения
            hit_cooldown = 100; // задержка перед следующим отражением
        }
    }
    hit_cooldown -= ball_speed;
    if (hit_cooldown < 0)
        hit_cooldown = 0;

    if (map.size() == 0) // условие победы
        return 1;


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
