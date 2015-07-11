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
    pause = false;
    dir_search = false;

    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
    ui->about->setEnabled(false);
    ui->about->setVisible(false);
    ui->status->setGeometry(width(),-100,10,10);

}

Widget::~Widget()
{
    delete ui;
    delete level;
    //delete img;
}

void Widget::timerEvent(QTimerEvent *t)
{
    if (game_running && !pause)
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
    QFont font;

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

    if (pause) // пауза
    {
        pen.setColor(Qt::yellow);
        font.setPixelSize(width()/14);
        p.setPen(pen);
        p.setFont(font);
        QString str = "Пауза";
        p.drawText(width()/2-str.length()*font.pixelSize()/4, height()/5*3+50, str);


    }

    if (game_over || game_win) // проигрыш
    {
        if (game_over)
            pen.setColor(Qt::red);
        else
            pen.setColor(Qt::green);
        p.setPen(pen);
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


    if (dir_search)
    {

        font.setPixelSize(40);
        p.setFont(font);
        p.setPen(pen);
        p.drawText(10,50,"Выберите папку:");

        font.setPixelSize(20);
        p.setFont(font);
        QRect rect;
        for (int i = 0; i < dirs_with_img.size(); i++)
        {
            pen.setColor(Qt::white);
            p.setPen(pen);
            rect.setTopLeft(QPoint(10+(i%4)*(width()/5+20),100+i/4*(width()/5+30)));
            rect.setHeight(width()/5);
            rect.setWidth(width()/5);

            p.drawPixmap(rect,pixmap_array.at(i));
            QString str = dirs_with_img.at(i).right(dirs_with_img.at(i).size()-1 - dirs_with_img.at(i).lastIndexOf("/"));
            p.drawText(20+(i%4)*(width()/5+20),90+i/4*(width()/5+30),str);
            p.drawRect(rect);

            /*pen.setColor(Qt::green);
            p.setPen(pen);
            p.drawRect(10+(i%4)*(width()/5+20),
                       100+(i/4)*(width()/5+20),
                       (width()/5),
                       (width()/5));*/
        }
    }

    if (image_search)
    {
        pen.setColor(Qt::white);
        font.setPixelSize(40);
        p.setFont(font);
        p.setPen(pen);
        p.drawText(10,50,"Выберите картинку для игры:");

        font.setPixelSize(20);
        p.setFont(font);
        QRect rect;
        for (int i = 0; i < images.size(); i++)
        {
            rect.setTopLeft(QPoint((i%5)*(width()/5+5),100+i/5*(width()/5+5)));
            rect.setHeight(width()/5);
            rect.setWidth(width()/5);

            p.drawPixmap(rect,pixmap_array.at(i));
            p.drawRect(rect);

        }
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
    ui->status->setGeometry(width(),-100,10,10);

    // для виндуса!!!
    /*QFileDialog fd;
    fd.setMaximumSize(width(), height());
    fd.setGeometry(30,0,width(), height());
    //fd.setSizePolicy();
    QString str = fd.getOpenFileName(0, "Choose Image", "", "*.png *.jpg");
    qDebug() << str << fd.geometry();
    if (str == "")
    {
        ui->start_button->pressed(); // пофиксить
        qDebug() << "lol";
    }
    img = new QImage(str);*/

    //!


    pixmap_array.clear();
    images.clear();

    dir_search = true;
    find_all_img("/sdcard");
    find_all_img("/storage/extSdCard");
    //find_all_img("D:/testdir");
    qDebug() << dirs_with_img;


}

void Widget::mousePressEvent(QMouseEvent *m)
{
    if (game_running && !pause)
        level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);

    if (dir_search)
    {
        for (int i = 0; i < dirs_with_img.size(); i++)
        {
            if (m->x() >= 10+(i%4)*(width()/5+20)
                    && m->x() <= 10+(i%4)*(width()/5+20) + width()/5
                    && m->y() >= 100+i/4*(width()/5+20)
                    && m->y() <= 100+i/4*(width()/5+20) + width()/5)
            {
                dir_search = false;
                image_search = true;
                qDebug() << "pressed" << i;
                find_all_img(dirs_with_img.at(i));
            }

        }
    }
    else if (image_search)
    {
        for (int i = 0; i < dirs_with_img.size(); i++)
        {
            if (m->x() >= 10+(i%4)*(width()/5+20)
                    && m->x() <= 10+(i%4)*(width()/5+20) + width()/5
                    && m->y() >= 100+i/4*(width()/5+20)
                    && m->y() <= 100+i/4*(width()/5+20) + width()/5)
            {
                dir_search = false;
                image_search = false;
                qDebug() << "choosed" << i;
                img = new QImage(images.at(i));
                pixmap_array.clear();
                images.clear();
                ui->status->setMaximum(level->load_map(img, width(), height())); // загрузка текущего уровня
                level->set_ball_coord(width()/2-level->get_board_width()/2, height()/5*4 - 5);
                level->set_ball_angle(55+rand()%70);
                level->set_board_coord(width()/2-width()/5, height()/5*4);  // начальные координат доски
                game_win = false;
                game_running = true;
            }

        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *m)
{
    if (game_running && !pause)
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
    // 16777313 - клавиша назад
    // 16777301 - клавиша функций
    if (k->key() == Qt::Key_Back)
    {
        if (game_running)
            pause = !pause;
        if (dir_search)
        {
            dir_search = false;
            menu_change_state(true);
        }
        if (image_search)
        {
            dir_search = true;
            image_search = false;
            on_start_button_clicked();
        }

    }

}

void Widget::find_all_img(QString start_dir)
{
    QDir dir;
    dir.cd(start_dir);

    QList <QString> filter_list;
    QStringList dir_list = dir.entryList(filter_list, QDir::Dirs);
    filter_list.append("");
    filter_list.append("*.jpg");
    filter_list.append("*.png");
    QStringList files_list = dir.entryList(filter_list, QDir::Files);

    if (dir_search)
    {
        for (int i = 0; i < dir_list.size(); i++)
        {
            if (dir_list.at(i) == ".")
                dir_list.takeAt(i);
            if (dir_list.at(i) == "..")
                dir_list.takeAt(i);
        }
        if (files_list.size() != 0)
        {
            dirs_with_img.append(start_dir);
            pixmap_array.append(QPixmap(start_dir+"/"+files_list.first()));
        }

        for (int i = 0; i < dir_list.size(); i++)
        {
            if (dir_list.at(i) == "cache"
                    || dir_list.at(i) == "data")
                continue;
            find_all_img(start_dir + "/" + dir_list.at(i));
        }
    }

    if (image_search)
    {
        pixmap_array.clear();
        images.clear();
        for (int i = 0; i < files_list.size(); i++)
        {
            images.append(QString(start_dir+"/"+files_list.at(i)));
            pixmap_array.append(QPixmap(start_dir+"/"+files_list.at(i)));
        }

    }

}
