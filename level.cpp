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
}

Level::Level(int w, int h, double angle)
{
    brick_size_x = 5;
    brick_size_y = 5;
    board_x = 300;
    board_y = 500;
    board_height = 20;
    board_width  = w/5;
    ball_x = 300;
    ball_y = 800;
    ball_speed = 1.5;
    ball_angle = angle;
    grid_x = 8;
    grid_y = 8;
    hit_cooldown = 0;
    ball_last_x = ball_x;
    ball_last_y = ball_y;
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

    board_width  = width/5;

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
    else if (ball_x >= width - (width - grid_x*brick_size_x)) // правая стенка
    {
        ball_angle = 180 - ball_angle + (rand()%2 - 1); // отражение + погрешность
        ball_x = width - (width - grid_x*brick_size_x) - 1;
    }
    else if (ball_y <= 0) //верхняя стенка
    {
        ball_angle = -ball_angle + (rand()%2); // отражение + погрешность
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
    if (((int)ball_angle%90) != 0) // если это не прямой угол
        k = tan(ball_angle * PI / 180.0);
    else k = 0;
    int hit = 0; // флаг столкновения

    for (int i = 0; i < map.size(); i++)
    {
        int brick_x = map[i].get_coord().x(); // координаты рассматриваемого кирпичика
        int brick_y = map[i].get_coord().y();
        if (ball_x >= brick_x
                && ball_x <= brick_x+brick_size_x
                && ball_y >= brick_y
                && ball_y <= brick_y+brick_size_y) // если шарик внутри крипичика
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

                // костыль для редкого, но неприятного бага
                if (brick_size_x - fabs(ball_x-(map[i].get_coord().x()+brick_size_x/2)) > fabs(ball_x-ball_last_x)*2
                   && brick_size_y - fabs(ball_y-(map[i].get_coord().y()+brick_size_y/2)) > fabs(ball_y-ball_last_y)*2)
                {
                    if (hit == 0) // если убрать, то уничтожение всего вкл
                    {
                    map.takeAt(i);
                    break;
                    }
                }


            }

            else
            {
                if (ball_angle == 0 || ball_angle == 180)
                    hit = 1;
                else if (ball_angle == 90 || ball_angle == 270)
                    hit = 2;
            }

            if (hit == 1)
            {
                ball_angle = 360 - (180 + ball_angle);
                ball_x = ball_last_x;
                ball_y = ball_last_y;
                map.takeAt(i);
            }
            else if (hit == 2)
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
            ball_angle += dx/board_width*100; // доп угол отражения

            if (ball_angle < 0) // приведение угла для подсчета
                ball_angle = 360 + ball_angle;
            ball_angle = (int)ball_angle % 360;

            if (ball_angle < 10) // поправки
                ball_angle = 10;
            else if (ball_angle > 170)
                ball_angle = 170;
            hit_cooldown = 100; // задержка перед следующим отражением
        }
    }
    hit_cooldown -= ball_speed;
    if (hit_cooldown < 0)
        hit_cooldown = 0;

    return map.size();
}


int Level::load_map(QImage* img, int w, int h)
{

    map.clear(); // очистка карты от остатков

    double ratio = (double)img->width()/(double)img->height(); // соотношение сторон


    int w_use = w - w/10;

    int least_grid_x = 0; // подходящая сетка
    int least_brick_size_x = 0; // подходящий размер
    grid_x = 8; // начальное значение сетки
    brick_size_x = 5; // начальный размер блока
    int wdiff = abs(w_use - brick_size_x*grid_x); // "просвет" между данными параметрами и стенкой
    int grid_max = 30; // максимальный размер сетки
    int grid_min = 8;
    if (img->width() < 30)
        grid_max = img->width();
    if (img->width() < 8)
        grid_min = img->width();

    if (grid_max == 30 && grid_min == 8)
    {
        for (grid_x = grid_min; grid_x <= grid_max; grid_x++) // выборка оптимальных размеров
        {
            for (brick_size_x = 5; brick_size_x <= w_use/grid_max*5; brick_size_x++)
            {
                 if (grid_x <= 20)
                    {
                        if (wdiff >= abs(w_use - (brick_size_x)*grid_x))
                        {
                            wdiff = abs(w_use - brick_size_x*grid_x);
                            least_brick_size_x = brick_size_x;
                            least_grid_x = grid_x;
                        }
                    }
                else
                        if (wdiff >= abs(w_use - (brick_size_x)*grid_x)) // пытаемся больше 20 не ставить
                        {
                            wdiff = abs(w_use - brick_size_x*grid_x);
                            least_brick_size_x = brick_size_x;
                            least_grid_x = grid_x;
                        }
            }
        }
    }
    else
    {
        least_grid_x = grid_max;
        least_brick_size_x = w_use/least_grid_x;
    }

    grid_x = least_grid_x; // запоминаем оптимальный размер
    brick_size_x = least_brick_size_x;


    grid_y = grid_x/ratio; // находим сетку по высоте


    if (brick_size_x/ratio*grid_y < h/5*3) // если слишком высокая, то урезаем
        brick_size_y = brick_size_x/ratio;
    else
        brick_size_y = h/5*3/grid_y;

    int scale_x = img->width()/grid_x; // количество пикселей, сжимаемых в 1 кирпичик
    int scale_y = img->height()/grid_y;


    QColor color; // цвет кипричика
    QColor rgb; // временный цвет
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;
    QImage alpha_channel; // прозрачность картинки
    QColor alpha;
    if (img->hasAlphaChannel())
        alpha_channel = img->alphaChannel();

    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
        {
            r = 0;
            g = 0;
            b = 0;
            a = 0;
            for (int m = i*scale_x; m < (i+1)*scale_x; m++)
                for (int n = j*scale_y; n < (j+1)*scale_y; n++)
                {
                    rgb = img->pixel(m,n); // узнаем цвет пикселя

                    r += rgb.red();
                    g += rgb.green();
                    b += rgb.blue();
                    if (img->hasAlphaChannel())
                    {
                        alpha = alpha_channel.pixel(m,n);
                        a += alpha.red(); // прозрачность, неважно какой из цветов брать
                    }
                }
            r = r/(scale_x*scale_y); // среднее значение для сжатия
            g = g/(scale_x*scale_y);
            b = b/(scale_x*scale_y);
            a = a/(scale_x*scale_y);
            if (img->hasAlphaChannel())
                color.setRgb(r, g, b, a);
            else
                color.setRgb(r, g, b);

            if (img->hasAlphaChannel())
            {
                if (a > 100) // если кирпичек не прозрачнее порога, то его добавляем
                {
                    Brick *brick;
                    brick = new Brick(QPoint(i*brick_size_x, j*brick_size_y), color);
                    map.push_front(*brick); // добавляем к карте новый кирпичик
                    delete brick;
                }
            }
            else
                {
                    Brick *brick;
                    brick = new Brick(QPoint(i*brick_size_x, j*brick_size_y), color);
                    map.push_front(*brick); // добавляем к карте новый кирпичик
                    delete brick;
                }


        }
    return map.size(); // возвращаем размер получившейся карты для статус бара
}
