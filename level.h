#ifndef LEVEL_H
#define LEVEL_H

#include <QDebug>
#include <cmath>

#include "brick.h"

class Level
{
private:
    short brick_size_x; // размер кирпичика
    short brick_size_y;
    short grid_x; // кол-во клеток по х
    short grid_y; // кол-во клеток по у

    int board_x; // параметры доски
    int board_y;
    int board_height;
    int board_width;

    double ball_x; // параметры шарика
    double ball_y;
    double ball_speed; // скорость
    double ball_angle; // угол движения


    QList <Brick> map; // карта

public:
    Level();
    Level(short x, short y, double angle);

    void load_map(); // создать карту

    void set_brick_size(int w, int h);
    void set_grid(int x, int y);
    void set_board_coord(int x, int y);
    void set_ball_coord(double x, double y);
    void set_ball_angle(int angle);
    int ball_update(int width, int height); // обработка шарика и его коллизий


    double get_ball_x() { return ball_x; }
    double get_ball_y() { return ball_y; }
    double get_ball_speed() { return ball_speed; }
    double get_ball_angle() { return ball_angle; }
    int get_board_x() { return board_x; }
    int get_board_y() { return board_y; }
    int get_board_width() { return board_width; }
    int get_board_height() { return board_height; }
    short get_brick_size_x() { return brick_size_x; }
    short get_brick_size_y() { return brick_size_y; }
    short get_grid_x() { return grid_x; }
    short get_grid_y() { return grid_y; }
    int get_map_size() { return map.size(); }
    QPoint get_brick_coord(int i) { return map[i].get_coord(); }
    QColor get_brick_color(int i) { return map[i].get_color(); }
};

#endif // LEVEL_H
