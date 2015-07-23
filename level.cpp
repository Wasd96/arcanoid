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
    ball.append(Ball(300 , 800, 1, 90));
    ball.last().set_last_x(ball.first().get_x());
    ball.last().set_last_y(ball.first().get_y());
    hit_cooldown.append(0);
    explosive = false;
    super_ball = false;
    bonus_width = false;
}

Level::Level(int w, int h, double angle)
{
    brick_size_x = 5;
    brick_size_y = 5;
    board_x = 300;
    board_y = 500;
    board_height = 20;
    board_width  = w/5;
    grid_x = 8;
    grid_y = 8;
    hit_cooldown.append(0);
    ball.append(Ball(300 , 800, 1, angle));
    ball.last().set_last_x(ball.first().get_x());
    ball.last().set_last_y(ball.first().get_y());
    explosive = false;
    super_ball = false;
    bonus_width = false;
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

void Level::set_ball_coord(double _x, double _y, int i)
{
    ball[i].set_x(_x);
    ball[i].set_y(_y);
}

void Level::set_ball_angle(int _angle, int i)
{
    ball[i].set_angle(_angle);
}

int Level::nearest(int distance, int ball_id)
{
    double lenx;
    double leny;
    for (int i = 0; i < map.size(); i++)
    {
        lenx = abs(ball[ball_id].get_x() - (map[i].get_coord().x() + brick_size_x/2));
        leny = abs(ball[ball_id].get_y() - (map[i].get_coord().y() + brick_size_y/2));
        if (sqrt(lenx*lenx + leny*leny) < distance)
        {
            return i;
        }

    }
    return -1;
}

void Level::double_ball()
{
    int cur_ball_size = ball.size();
    for (int i = 0; i < cur_ball_size; i++)
    {
        ball[i].set_angle(ball[i].get_angle()-45);
        ball.append(Ball(ball[i].get_x(),
                         ball[i].get_y(),
                         ball[i].get_speed(),
                         ball[i].get_angle()+90));
        hit_cooldown.append(0);
    }
}

void Level::update_img(QImage *image_brick)
{
    QTime imgupd;
    imgupd.start();

    image_brick->fill(Qt::black);
    int i;
    int j;
    int k;
    int brick_x;
    int brick_y;
    int br_size_x = brick_size_x;
    int br_size_y = brick_size_y;
    QRgb color;
    QRgb color_border = QColor(150,150,150).rgb();
    int m_size = map.size();
    for (i = 0; i < m_size; i++)
    {
        brick_x = get_brick_coord(i).x();
        brick_y = get_brick_coord(i).y();
        color = get_brick_color(i).rgb();
        for (j = brick_x; j <= brick_x + br_size_x; j++)
        {
            for (k = brick_y; k <= brick_y + br_size_y; k++)
            {

                image_brick->setPixel(j,k,color);
                if (j == brick_x
                        || k == brick_y
                        || j == brick_x + br_size_x
                        || k == brick_y + br_size_y)
                    image_brick->setPixel(j,k,color_border);
            }
        }

    }

    qDebug() << imgupd.elapsed();
}

int Level::update(int width, int height, QImage *image_brick)
{
    double PI = 3.14159265;

    if (image_brick->pixel(0,0) == QColor(0,0,1).rgb())
    {
        update_img(image_brick);

    }


    int hit = 0; // флаг столкновения
    for (int i = 0; i < ball.size(); i++)
    {
        if (ball[i].get_angle() < 0) // приведение угла для подсчета
            ball[i].set_angle(360 + ball[i].get_angle());
        ball[i].set_angle((int)ball[i].get_angle() % 360);

        ball[i].set_speed(height/200); // скорость


        if (ball[i].get_x() <= 0) // левая стенка
        {
            ball[i].set_angle(180 - ball[i].get_angle() + (rand()%3)); // отражение + погрешность
            ball[i].set_x(1);
        }
        else if (ball[i].get_x() >= width - (width - grid_x*brick_size_x)) // правая стенка
        {
            ball[i].set_angle(180 - ball[i].get_angle() + (rand()%2 - 1)); // отражение + погрешность
            ball[i].set_x(width - (width - grid_x*brick_size_x) - 1);
        }
        else if (ball[i].get_y() <= 0) //верхняя стенка
        {
            ball[i].set_angle(-ball[i].get_angle() + (rand()%2)); // отражение + погрешность
            ball[i].set_y(1);
        }
        else if (ball[i].get_y() >= height) // проигрыш (нижняя стенка)
        {
            if (ball.size() == 1)
                return -1;
            else
            {
                ball.takeAt(i);
                return -2;
            }
        }


        ball[i].set_last_x(ball[i].get_x()); // старые координаты
        ball[i].set_last_y(ball[i].get_y());
        ball[i].set_x(ball[i].get_x() - ball[i].get_speed() * cos(ball[i].get_angle() * PI / 180.0)); // новые координаты
        ball[i].set_y(ball[i].get_y() - ball[i].get_speed() * sin(ball[i].get_angle() * PI / 180.0));

        // столкновения с кубиками
        double k = 0; // коэффициент для ур-ния прямой
        double b = 0; // коэффициент для ур-ния прямой
        double cross_y_left = 0;    // точка столкновения с левой гранью
        double cross_y_right = 0;   // точка столкновения с правой гранью
        double cross_x_bottom = 0;  // точка столкновения с нижней гранью
        double cross_x_top = 0;     // точка столкновения с верхней гранью
        if (((int)ball[i].get_angle()%90) != 0) // если это не прямой угол
            k = tan(ball[i].get_angle() * PI / 180.0);
        else k = 0;

        if ((int)(ball[i].get_x()/brick_size_x) < grid_x && (int)(ball[i].get_y()/brick_size_y) < grid_y)
        if (map_colliders[(int)(ball[i].get_x()/brick_size_x)][(int)(ball[i].get_y()/brick_size_y)])
        {
            for (int j = 0; j < map.size(); j++)
            {
                int brick_x = map[j].get_coord().x(); // координаты рассматриваемого кирпичика
                int brick_y = map[j].get_coord().y();
                if (ball[i].get_x()>= brick_x
                        && ball[i].get_x()<= brick_x+brick_size_x
                        && ball[i].get_y()>= brick_y
                        && ball[i].get_y()<= brick_y+brick_size_y) // если шарик внутри крипичика
                {
                    if (super_ball)
                    {
                        map.takeAt(j);
                        update_img(image_brick);
                        map_colliders[(int)(ball[i].get_x()/brick_size_x)][(int)(ball[i].get_y()/brick_size_y)] = 0;
                        return map.size();
                    }

                    b = ball[i].get_last_y() - ((ball[i].get_y() - ball[i].get_last_y())/(ball[i].get_x() - ball[i].get_last_x()))*ball[i].get_last_x();

                    cross_x_bottom = ((brick_y + brick_size_y) - b)/k; // точки пересечений с гранями
                    cross_x_top = (brick_y - b)/k;
                    cross_y_right = k*(brick_x + brick_size_x - ball[i].get_x()) + ball[i].get_y();
                    cross_y_left = k*(brick_x - ball[i].get_x()) + ball[i].get_y();

                    if ((int)ball[i].get_angle()%90 != 0)
                    {
                        if (cross_x_bottom <= brick_x + brick_size_x
                                && cross_x_bottom >= brick_x) // низ
                        {
                            if (ball[i].get_angle() > 0 && ball[i].get_angle() < 90) // справа снизу
                                if (cross_x_bottom <= ball[i].get_last_x() && cross_x_bottom >= ball[i].get_x())
                                    hit = 2;
                            if (ball[i].get_angle() > 90 && ball[i].get_angle() < 180) // слева снизу
                                if (cross_x_bottom >= ball[i].get_last_x() && cross_x_bottom <= ball[i].get_x())
                                    hit = 2;
                        }

                        if (cross_y_left <= brick_y + brick_size_y
                                && cross_y_left >= brick_y) // лево
                        {
                            if (ball[i].get_angle() > 90 && ball[i].get_angle() < 180) // слева снизу
                                if (cross_y_left <= ball[i].get_last_y() && cross_y_left >= ball[i].get_y())
                                    hit = 1;
                            if (ball[i].get_angle() > 180 && ball[i].get_angle() < 270) // слева сверху
                                if (cross_y_left >= ball[i].get_last_y() && cross_y_left <= ball[i].get_y())
                                    hit = 1;
                        }

                        if (cross_y_right <= brick_y + brick_size_y
                                && cross_y_right >= brick_y) // право
                        {
                            if (ball[i].get_angle() > 0 && ball[i].get_angle() < 90) // справа снизу
                                if (cross_y_right <= ball[i].get_last_y() && cross_y_right >= ball[i].get_y())
                                    hit = 1;
                            if (ball[i].get_angle() > 270 && ball[i].get_angle() < 360) // справа сверху
                                if (cross_y_right >= ball[i].get_last_y() && cross_y_right <= ball[i].get_y())
                                    hit = 1;
                        }
                        if (cross_x_top <= brick_x + brick_size_x
                                && cross_x_top >= brick_x) // верх
                        {
                            if (ball[i].get_angle() > 270 && ball[i].get_angle() < 360) // справа сверху
                                if (cross_x_top <= ball[i].get_last_x() && cross_x_top >= ball[i].get_x())
                                    hit = 2;
                            if (ball[i].get_angle() > 180 && ball[i].get_angle() < 270) // слева сверху
                                if (cross_x_top >= ball[i].get_last_x() && cross_x_top <= ball[i].get_x())
                                    hit = 2;
                        }

                        // костыль для редкого, но неприятного бага
                        if (brick_size_x - fabs(ball[i].get_x() - (map[j].get_coord().x()+brick_size_x/2)) > fabs(ball[i].get_x() - ball[i].get_last_x())*2
                           && brick_size_y - fabs(ball[i].get_y() - (map[j].get_coord().y()+brick_size_y/2)) > fabs(ball[i].get_y() - ball[i].get_last_y())*2)
                        {
                            if (hit == 0)
                            {
                                map.takeAt(j);
                                map_colliders[(int)(ball[i].get_x()/brick_size_x)][(int)(ball[i].get_y()/brick_size_y)] = 0;
                            }
                        }


                    }

                    else
                    {
                        if (ball[i].get_angle() == 0 || ball[i].get_angle() == 180)
                            hit = 1;
                        else if (ball[i].get_angle() == 90 || ball[i].get_angle() == 270)
                            hit = 2;
                    }

                    if (hit)
                    {
                        if (hit == 1)
                            ball[i].set_angle(360 - (180 + ball[i].get_angle()));
                        if (hit == 2)
                            ball[i].set_angle(-ball[i].get_angle());

                        if (!explosive)
                            map.takeAt(j);
                        else
                        {
                            int near = nearest(sqrt(brick_size_x*brick_size_x+brick_size_y*brick_size_y), i);
                            while (near != -1)
                            {
                                map.takeAt(near);
                                near = nearest(sqrt(brick_size_x*brick_size_x+brick_size_y*brick_size_y), i);
                            }
                        }
                        map_colliders[(int)(ball[i].get_x()/brick_size_x)][(int)(ball[i].get_y()/brick_size_y)] = 0;
                        ball[i].set_x(ball[i].get_last_x());
                        ball[i].set_y(ball[i].get_last_y());
                    }

                }
            }
        }


        // столкновение с доской
        double dx = ball[i].get_x() - (board_x + board_width/2); // растояние от шарика до центра доски
        double dy = ball[i].get_y() - (board_y + board_height/2);
        if (hit_cooldown.at(i) == 0) // если отражение возможно
        {
            if (abs(dx) < board_width/2 && abs(dy) < board_height/2) // столкновение
            {
                ball[i].set_x(ball[i].get_last_x()); // возврат позиции
                ball[i].set_y(ball[i].get_last_y()); // шарика
                ball[i].set_angle(-ball[i].get_angle()); // отражение
                ball[i].set_angle(ball[i].get_angle() + dx/board_width*100); // доп угол отражения

                if (ball[i].get_angle() < 0) // приведение угла для подсчета
                    ball[i].set_angle(360 + ball[i].get_angle());
                ball[i].set_angle((int)ball[i].get_angle() % 360);

                if (ball[i].get_angle() < 10) // поправки
                    ball[i].set_angle(10);
                else if (ball[i].get_angle() > 170)
                    ball[i].set_angle(170);
                hit_cooldown[i] = 100; // задержка перед следующим отражением
            }
        }

        hit_cooldown[i] -= ball[i].get_speed();
        if (hit_cooldown.at(i) < 0)
            hit_cooldown[i] = 0;

    }

    if (hit)
    {
        update_img(image_brick);
        return map.size();
    }

    return -2;
}


int Level::load_map(QImage* img, int w, int h)
{

    board_width = w/6;

    while(ball.size()-1 > 0)
        ball.takeLast();

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
    if (img->width() < 50)
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
    if (brick_size_y < 10)
        brick_size_y = 10;

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
    map_colliders = new int*[grid_x];
    for (int i = 0; i < grid_x; i++)
        map_colliders[i] = new int[grid_y];

    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            map_colliders[i][j] = 0;

    for (int i = 0; i < grid_x; i++)
    {
        for (int j = 0; j < grid_y; j++)
        {
            for (int k = 0; k < map.size(); k++)
            {
                int brick_x = map[k].get_coord().x();
                int brick_y = map[k].get_coord().y();
                if (brick_x == brick_size_x*i && brick_y == brick_size_y*j)
                {
                    map_colliders[i][j] = 1;
                    break;
                }

            }

        }
    }

    return map.size(); // возвращаем размер получившейся карты для статус бара
}
