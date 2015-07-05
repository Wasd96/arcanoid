#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    menu_change_state(true);
    srand(time(NULL)); // инициализация рандома

    level = new Level(5,5,-55-rand()%70); // инициализация основного класса игры

    //showFullScreen(); // все равно не работает на телефоне

    timer = startTimer(3); // таймер обработки шарика и отрисовки
    game_running = false;
    game_over = false;

    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
}

Widget::~Widget()
{
    delete ui;
    delete level;
}

void Widget::timerEvent(QTimerEvent *t)
{
    if (game_running)
    {
        int state = level->update(width(), height()); // состояние игрового мира
        if (state == -1) // проигрыш
        {
            game_over = true;
            game_running = false;
        }
        else if (state = 0);



    }

    repaint();

}

void Widget::paintEvent(QPaintEvent *ev) // отрисовка
{
    QPainter p(this);
    QPen pen;

    // отрисовка элементов
    if (!game_running)
    {
        ui->start_button->setGeometry(width()/2 - ui->start_button->width()/2,
                                      height()/7*4,width()/7*4, height()/10);
        ui->about_button->setGeometry(width()/2 - ui->about_button->width()/2,
                               height()/7*5, width()/3, height()/10);
        ui->exit_button->setGeometry(width()/2 - ui->exit_button->width()/2,
                                      height()/7*6, width()/3, height()/10);
    }

    if (game_running || game_over)
    {
        // отрисовка уровня
        p.fillRect(0,height()/2,width(),height(),Qt::black);
        for (int i = 0; i < level->get_map_size(); i++)
        {
            pen.setColor(level->get_brick_color(i));
            p.setPen(pen);
            p.fillRect(level->get_brick_coord(i).x(), // рисуем кирпичик
                       level->get_brick_coord(i).y(),
                       level->get_brick_size_x(),
                       level->get_brick_size_y(),
                       QColor(level->get_brick_color(i)));
        }

        // отрисовка доски
        QBrush brush;
        brush.setColor(Qt::red);
        brush.setStyle(Qt::SolidPattern);
        p.setBrush(brush);
        p.drawChord(level->get_board_x(), level->get_board_y(),
                    level->get_board_width(), level->get_board_height(),
                    0*16, 180*16);
        p.fillRect(level->get_board_x(), level->get_board_y() + level->get_board_height()/2,
                   level->get_board_width(), level->get_board_height()/2,
                   QColor(Qt::red));

        // отрисовка шарика
        pen.setColor(Qt::green);
        pen.setWidth(5);
        p.setPen(pen);
        p.drawEllipse(level->get_ball_x(), level->get_ball_y(), 5, 5);
    }

    if (game_over) // проигрыш
    {
        pen.setColor(Qt::red);
        p.setPen(pen);
        QFont font;
        font.setPixelSize(2);
        p.drawText(width()/4, height()/2, "Game Over");
        ui->menu->setGeometry(0, height()-height()/10, width()/4, height()/10);
        ui->menu->setVisible(true);
        ui->menu->setEnabled(true);
    }

}

void Widget::menu_change_state(bool state)
{
    // свернуть/показать меню
    ui->start_button->setEnabled(state);
    ui->exit_button->setEnabled(state);
    ui->about_button->setEnabled(state);

    ui->start_button->setVisible(state);
    ui->exit_button->setVisible(state);
    ui->about_button->setVisible(state);
}

void Widget::on_exit_button_clicked()
{
    delete ui;
    delete level;
    exit(0);
}

void Widget::on_start_button_clicked()
{
    menu_change_state(false);
    level->set_grid(25,15); // создание сетки
    level->set_brick_size(width(), height()); // задание размеров блока
    level->load_map(); // загрузка текущего уровня
    level->set_ball_coord(width()/2-level->get_board_width()/2, height()/5*4 - 5);
    level->set_ball_angle(-55-rand()%70);
    level->set_board_coord(width()/2-level->get_board_width()/2, height()/5*4);  // начальные координат доски
    game_running = true;
}

void Widget::mousePressEvent(QMouseEvent *m)
{
    if (game_running)
        level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);
    ui->label->setNum(m->x()); // тестовые значения
    ui->label_2->setNum(m->y());
}

void Widget::mouseMoveEvent(QMouseEvent *m)
{
    if (game_running)
        level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);
    ui->label->setNum(level->get_ball_angle());
    ui->label_2->setNum(level->get_ball_speed());
}

void Widget::on_menu_clicked()
{
    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
    game_over = false;
    menu_change_state(true);
}
