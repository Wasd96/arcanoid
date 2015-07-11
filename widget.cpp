#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    menu_change_state(true); // отображаем меню
    srand(time(NULL)); // инициализация рандома

    level = new Level(width(), height(), 55+rand()%70); // инициализация основного класса игры

    timer = startTimer(5); // таймер обработки шарика и отрисовки
    game_running = false;
    game_over = false;
    game_win = false;

    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
    ui->about->setEnabled(false);
    ui->about->setVisible(false);
    ui->status->setGeometry(width()-width()/10,0,width()/10,height());

}

Widget::~Widget()
{
    delete ui;
    delete level;
    //delete img;
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
        else if (state == 0) // победа
        {
            ui->status->setValue(ui->status->maximum() - state);
            game_win = true;
            game_running = false;
        }
        else if (state != 0)
        {
            ui->status->setValue(ui->status->maximum() - state);

            int item = rand()%10; // бонусы (А надо ли?)
            if (item == 1)
            {
                ;
            }
        }



    }

    repaint();

}

void Widget::paintEvent(QPaintEvent *ev) // отрисовка
{
    QPainter p(this);
    QPen pen;
    QBrush brush;

    // отрисовка элементов
    if (!game_running)
    {
        ui->start_button->setGeometry(width()/2 - ui->start_button->width()/2,
                                      height()/9*3,width()/7*4, height()/9);
        ui->settings_button->setGeometry(width()/2 - ui->settings_button->width()/2,
                                         height()/7*4, width()/3, height()/10);
        ui->about_button->setGeometry(width()/2 - ui->about_button->width()/2,
                               height()/7*5, width()/3, height()/10);
        ui->exit_button->setGeometry(width()/2 - ui->exit_button->width()/2,
                                      height()/7*6, width()/3, height()/10);
    }

    if (game_running || game_over || game_win)
    {
        // отрисовка уровня
        p.fillRect(0,height()/2,width(),height(),Qt::black);
        for (int i = 0; i < level->get_map_size(); i++)
        {
            p.fillRect(level->get_brick_coord(i).x(), // рисуем кирпичик
                       level->get_brick_coord(i).y(),
                       level->get_brick_size_x(),
                       level->get_brick_size_y(),
                       QColor(level->get_brick_color(i)));
            pen.setColor(Qt::gray);
            p.setPen(pen);
            p.drawRect(level->get_brick_coord(i).x(), // рисуем контур
                       level->get_brick_coord(i).y(),
                       level->get_brick_size_x(),
                       level->get_brick_size_y());
        }

        // отрисовка доски
        pen.setColor(Qt::yellow);
        brush.setColor(Qt::red);
        brush.setStyle(Qt::SolidPattern);
        p.setBrush(brush);
        p.setPen(pen);
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


        ui->status->setGeometry(width()-(width()-level->get_grid_x()*level->get_brick_size_x()),
                                0,width()-level->get_grid_x()*level->get_brick_size_x(),height());
    }

    if (game_over || game_win) // проигрыш
    {
        if (game_over)
            pen.setColor(Qt::red);
        else
            pen.setColor(Qt::green);
        p.setPen(pen);
        QFont font;
        font.setPixelSize(width()/14);
        p.setFont(font);
        QString str;
        if (game_over)
            str = "Поражение :(";
        else
            str = "Победа! :)";

        if (game_win) // отрисовка изображения
        {
            QRect rect;
            int w = width() - ui->status->width();
            double h = ((double)img->width()/(double)img->height())*(double)w;
            if (h > height()/5*3)
                h = height()/5*3;
            rect.setRect(0,0,w,h);
            p.drawImage(rect, *img);
        }
        p.drawText(width()/2-str.length()*font.pixelSize()/4, height()/5*3+50, str);
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
    ui->settings_button->setEnabled(state);
    ui->status->setEnabled(!state);

    ui->start_button->setVisible(state);
    ui->exit_button->setVisible(state);
    ui->about_button->setVisible(state);
    ui->settings_button->setVisible(state);
    ui->status->setVisible(!state);
}

void Widget::on_exit_button_clicked()
{
    delete ui;
    delete level;
    //delete img; // исправить !!!
    exit(0);
}

void Widget::on_start_button_clicked()
{
    menu_change_state(false);
    ui->status->setValue(0);

    //для виндуса!!!
    QString str = QFileDialog::getOpenFileName(0, "Choose Image", "", "*.png *.jpg");
    qDebug() << str;
    img = new QImage(str);
    //!!
    ui->status->setMaximum(level->load_map(img, width(), height())); // загрузка текущего уровня
    level->set_ball_coord(width()/2-level->get_board_width()/2, height()/5*4 - 5);
    level->set_ball_angle(55+rand()%70);
    level->set_board_coord(width()/2-width()/5, height()/5*4);  // начальные координат доски
    game_win = false;
    game_over = false;
    game_running = true;
}

void Widget::mousePressEvent(QMouseEvent *m)
{
    if (game_running)
        level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);
}

void Widget::mouseMoveEvent(QMouseEvent *m)
{
    if (game_running)
        level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);
}

void Widget::on_menu_clicked() // в меню
{
    ui->status->setVisible(false);
    ui->about->setEnabled(false);
    ui->about->setVisible(false);
    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
    game_over = false;
    game_win = false;
    game_running = false;
    menu_change_state(true);
}

void Widget::on_about_button_clicked()
{
    menu_change_state(false);
    ui->status->setVisible(false);
    ui->about->setEnabled(true);
    ui->about->setVisible(true);
    ui->about->setGeometry(width()/20, height()/20, width()/20*18, height()/20*14);
    ui->about->setText("Это текст. \n с переносом.");

    ui->menu->setGeometry(0, height()-height()/10, width()/4, height()/10);
    ui->menu->setVisible(true);
    ui->menu->setEnabled(true);
}

void Widget::on_settings_button_clicked()
{
    menu_change_state(false);
    ui->status->setVisible(false);


    ui->menu->setGeometry(0, height()-height()/10, width()/4, height()/10);
    ui->menu->setVisible(true);
    ui->menu->setEnabled(true);
}

void Widget::keyPressEvent(QKeyEvent *k)
{
    qDebug() << k->key();
}
