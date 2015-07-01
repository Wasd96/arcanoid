#ifndef BRICK_H
#define BRICK_H

#include <QColor>
#include <QPoint>

class Brick
{
private:
    QPoint coord; // координаты
    QColor color; // цвет


public:
    Brick();
    Brick(QPoint _point, QColor _color);
    ~Brick();

    //short get_x() { return x; }
    //short get_y() { return y; }
    QPoint get_coord() { return coord; }
    QColor get_color() { return color; }
};

#endif // BRICK_H
