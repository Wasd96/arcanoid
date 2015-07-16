#ifndef BALL_H
#define BALL_H


class Ball
{
private:
    double x; // параметры шарика
    double y;
    double last_x; // костыль для системы отражений
    double last_y; // содержит предыдущюю позицию шарика
    double speed; // скорость
    double angle; // угол движения
public:
    Ball();
    Ball(double _x, double _y, double _speed, double _angle);

    double get_x() { return x; }
    double get_y() { return y; }
    double get_angle() {return angle; }
    double get_speed() { return speed; }
    double get_last_x() { return last_x; }
    double get_last_y() { return last_y; }

    void set_x(double _x) { x = _x; }
    void set_y(double _y) { y = _y; }
    void set_angle(double _angle) { angle = _angle; }
    void set_speed(double _speed) { speed = _speed; }
    void set_last_x(double _x) { last_x = _x; }
    void set_last_y(double _y) { last_y = _y; }
};

#endif // BALL_H
