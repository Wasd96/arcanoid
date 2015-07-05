#ifndef WIDGET_H
#define WIDGET_H

#include "level.h"

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

    void menu_change_state(bool state); // спрятать/открыть меню

    void paintEvent(QPaintEvent *ev);

    void timerEvent(QTimerEvent *t);

    void mousePressEvent(QMouseEvent *m);

    void mouseMoveEvent(QMouseEvent *m);

private slots:

    void on_exit_button_clicked();

    void on_start_button_clicked();

    void on_menu_clicked();

private:
    Ui::Widget *ui; // Qt-шный виджет

    Level *level;

    int timer; // таймер обработки событий
    bool game_running; // игра запущена
    bool game_over; // игра проиграна
};

#endif // WIDGET_H
