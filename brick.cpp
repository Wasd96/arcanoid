#include "brick.h"

Brick::Brick()
{

}

Brick::Brick(QPoint _point, QColor _color) // создание крипичика
{
    coord = _point;
    color = _color;
}

Brick::~Brick()
{

}
