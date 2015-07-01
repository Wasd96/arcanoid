#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
//    setLayout(ui->gridLayout);


    menu_change_state(true);


}

Widget::~Widget()
{
    delete ui;
    //delete core;
}

void Widget::paintEvent(QPaintEvent *ev) // отрисовка
{
    QPainter p(this);

    //отрисовка элементов
    ui->start_button->setGeometry(width()/2 - ui->start_button->width()/2,
                                  height()/7*5, 300, 100);

    //



    Level level = *core.get_level();

    p.fillRect(height()/2,0,width(),height(),Qt::black);
    QPen pen;
    pen.setWidth(5);
    ui->label->setNum(level.get_map_size());
    for (int i = 0; i < level.get_map_size(); i++)
    {
        pen.setColor(level.get_brick_color(i));
        p.setPen(pen);
        p.drawPoint(level.get_brick_coord(i));
    }


}

void Widget::menu_change_state(bool state)
{

    ui->start_button->setEnabled(state);
    ui->exit_button->setEnabled(state);

    ui->start_button->setVisible(state);
    ui->exit_button->setVisible(state);
}

void Widget::on_exit_button_clicked()
{
    exit(0);
}

void Widget::on_start_button_clicked()
{
    menu_change_state(false);
    core.start_game(width(), height());
    //repaint();
}

void Widget::mousePressEvent(QMouseEvent *m)
{
    ui->label->setNum(m->x());
    ui->label_2->setNum(m->y());
}
