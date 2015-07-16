#ifndef WIDGET_H
#define WIDGET_H

#include "level.h"

#include <time.h>
#include <QDataStream>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QTime>

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

    void keyPressEvent(QKeyEvent *k);

    int find_all_img(QString start_dir, bool check);

private slots:

    void on_exit_button_clicked();

    void on_start_button_clicked();

    void on_menu_clicked();

    void on_about_button_clicked();

    void on_settings_button_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_refresh_cache_clicked();

private:
    Ui::Widget *ui; // Qt-шный виджет

    Level *level;

    int timer; // таймер обработки событий
    bool game_running; // игра запущена
    bool game_over; // игра проиграна
    bool game_win; // победа
    bool pause; // пауза
    bool dir_search;
    bool image_search;

    int bonus_width;
    int bonus_explosive;
    int bonus_super_ball;

    QColor opacity_color;
    QString opacity_text;

    QStringList dirs_with_img;
    QList <QPixmap> pixmap_array;
    QStringList images;
    QList <QString> filter_list;
    QDir dir;

    QStringList cache_dirs_with_img;
    QList <QPixmap> cache_pixmap_array;
    QList <QString> cache_img_time;

    QImage *img; // карта-картинка
};

#endif // WIDGET_H
