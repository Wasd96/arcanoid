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
    ball_y = 400;
    ball_speed = 5;
    ball_angle = 90;
    map.append(Brick(QPoint(0,0),QColor(0,0,0)));
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
    ball_y = 400;
    ball_speed = 10;
    ball_angle = angle;
    grid_x = x;
    grid_y = y;
    map.append(Brick(QPoint(0,0),QColor(0,0,0)));
}

void Level::set_brick_size(int w, int h)
{
    brick_size_x = w / grid_x;
    brick_size_y = h/2 / grid_y; //чтобы не занимало все пространсвто
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

    ball_angle = (int)ball_angle % 360;

    ball_x += ball_speed * cos(ball_angle * PI / 180.0);
    ball_y += ball_speed * sin(ball_angle * PI / 180.0);

    if (ball_x <= 0)
    {
        ball_angle = 180 - ball_angle;
        ball_x = 1;
    }
    else if (ball_x >= width)
    {
        ball_angle = 180 - ball_angle;
        ball_x = width - 1;
    }
    if (ball_y <= 0)
    {
        ball_angle = -ball_angle;
        ball_y = 1;
    }
    if (ball_y >= height)
    {
        ball_angle = -ball_angle;
        ball_y = height - 1;
    }

    if
}


void Level::load_map()
{


    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++) {
            Brick *brick = new Brick(QPoint(i*brick_size_x, j*brick_size_y),
                                    QColor(i*100%255, j*100%255, (i+j)*100%255));
            map.push_front(*brick);
            delete brick;
        }

}
