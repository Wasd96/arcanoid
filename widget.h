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

    int find_all_img(QString start_dir, bool check); // поиск изображений

private slots:

    void on_exit_button_clicked();

    void on_start_button_clicked();

    void on_menu_clicked();

    void on_about_button_clicked();

    void on_settings_button_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_refresh_cache_clicked();

    void resizeEvent(QResizeEvent *);

private:
    Ui::Widget *ui; // Qt-шный виджет

    Level *level;

    int timer; // таймер обработки событий
    bool game_running; // игра запущена
    bool game_over; // игра проиграна
    bool game_win; // победа
    bool pause; // пауза
    bool dir_search; // флаг поиска папок
    bool image_search; // флаг поиска картинок
    bool settings; // флаг настроек
    bool about; // флаг "об игре"
    bool first_time; // флаг первого захода
    bool first_resize; // костыль для системы анти-поворота
    bool turn_back; // требовние повернуть телефон
    int old_w; // костыль для системы анти-поворота
    int old_h; // костыль для системы анти-поворота

    int bonus_width; //бонус широкой доски
    int bonus_explosive; // бонус взрывного шара
    int bonus_super_ball; // бонус пробивающего щара

    bool loading; // флаг загрузки
    bool refreshing; // флаг обновления кэша
    QColor opacity_color; // для отображения бонусов
    QString opacity_text; // для отображения бонусов

    QStringList dirs_with_img; // список папок с картинками
    QList <QPixmap> pixmap_array; // список картинок
    QStringList images; // список имен картинок
    QList <QString> filter_list; // фильтр поиска
    QDir dir; // действующая папка

    QStringList cache_dirs_with_img; // кэш
    QList <QPixmap> cache_pixmap_array;
    QList <QString> cache_img_time;
    QList <QList <QPixmap> > cache_images_in_dirs;

    QImage *img; // картинка
    QImage *image_brick; // сжатая, игровая картинка
};

#endif // WIDGET_H
