#ifndef LEVEL_H
#define LEVEL_H

#include <QDebug>
#include <cmath>
#include <QImage>
#include <QRgb>
#include <QTime>

#include "brick.h"
#include "ball.h"

class Level
{
private:
    int brick_size_x; // размер кирпичика
    int brick_size_y;
    int grid_x; // кол-во клеток по х
    int grid_y; // кол-во клеток по у

    int board_x; // параметры доски
    int board_y;
    int board_height;
    int board_width;

    QList <Ball> ball; // список шариков

    QList <int> hit_cooldown; // задержка удара

    QList <Brick> map; // карта
    int** map_colliders; // карта столкновений для проверки столкновений

    bool explosive; // бонусы
    bool super_ball;
    bool bonus_width;

public:
    Level();
    Level(int w, int h, double angle);

    int load_map(QImage* img, int w, int h); // создать карту
    int nearest(int distance, int ball_id); // ближайший блок в радиусе distance

    void set_board_coord(int x, int y);
    void set_ball_coord(double _x, double _y, int i);
    void set_ball_angle(int _angle, int i);
    void set_board_width(int width) { board_width = width; }
    void set_explosive(bool exp) { explosive = exp; }
    void set_super_ball(bool sb) { super_ball = sb; }
    void set_bonus_width(bool wd) { bonus_width = wd; }

    void double_ball(); // удвоить кол-во шаров

    int update(int width, int height, QImage *image_brick); // итерация игрового мира
    void update_img(QImage *image_brick); // перерисовать изображение

    double get_ball_x(int i) { return ball[i].get_x(); }
    double get_ball_y(int i) { return ball[i].get_y(); }
    double get_ball_speed(int i) { return ball[i].get_speed(); }
    double get_ball_angle(int i) { return ball[i].get_angle(); }

    int get_board_x() { return board_x; }
    int get_board_y() { return board_y; }
    int get_board_width() { return board_width; }
    int get_board_height() { return board_height; }
    int get_brick_size_x() { return brick_size_x; }
    int get_brick_size_y() { return brick_size_y; }
    int get_grid_x() { return grid_x; }
    int get_grid_y() { return grid_y; }
    int get_map_size() { return map.size(); }
    int get_ball_counter() { return ball.size(); }
    bool get_explosive() { return explosive; }
    bool get_super_ball() { return super_ball; }
    bool get_bonus_width() { return bonus_width; }
    QPoint get_brick_coord(int i) { return map[i].get_coord(); }
    QColor get_brick_color(int i) { return map[i].get_color(); }
};

#endif // LEVEL_H
