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
    Brick(QPoint _point, QColor _color); // создание с заданными параметрами
    ~Brick(); // не знаю зачем, деструктор

    QPoint get_coord() { return coord; } // получить кооадинаты
    QColor get_color() { return color; } // получить цвет
};

#endif // BRICK_H
