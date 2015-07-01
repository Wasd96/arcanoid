#ifndef WIDGET_H
#define WIDGET_H

#include "level.h"

#include <cmath>
#include <time.h>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void menu_change_state(bool state);

    void paintEvent(QPaintEvent *ev);

    void timerEvent(QTimerEvent *t);

    void mousePressEvent(QMouseEvent *m);

    void mouseMoveEvent(QMouseEvent *m);

    void start_game();

private slots:

    void on_exit_button_clicked();

    void on_start_button_clicked();

private:
    Ui::Widget *ui;

    Level *level;

    int timer;
    bool game_running;
};

#endif // WIDGET_H
