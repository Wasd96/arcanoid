#ifndef WIDGET_H
#define WIDGET_H

#include "core.h"

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

    //void repaint();

    void mousePressEvent(QMouseEvent *m);

private slots:

    void on_exit_button_clicked();

    void on_start_button_clicked();

private:
    Ui::Widget *ui;

    Core core;
};

#endif // WIDGET_H
