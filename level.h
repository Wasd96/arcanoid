#ifndef LEVEL_H
#define LEVEL_H

#include "brick.h"

class Level
{
private:
    short brick_size_x; // размер кирпичика
    short brick_size_y;
    short grid_x; // кол-во клеток по х
    short grid_y; // кол-во клеток по у

    QList <Brick> map; // карта

public:
    Level();
    Level(short x, short y);

    void load_map(int curr_level);

    void set_brick_size(int w, int h);
    void set_grid(int x, int y);

    short get_brick_size_x() { return brick_size_x; }
    short get_brick_size_y() { return brick_size_y; }
    short get_grid_x() { return grid_x; }
    short get_grid_y() { return grid_y; }
    int get_map_size() { return map.size(); }
    QPoint get_brick_coord(int i) { return map[i].get_coord(); }
    QColor get_brick_color(int i) { return map[i].get_color(); }
};

#endif // LEVEL_H
