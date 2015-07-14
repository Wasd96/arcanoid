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

    timer = startTimer(15); // таймер обработки шарика и отрисовки
    game_running = false;
    game_over = false;
    game_win = false;
    pause = false;
    dir_search = false;
    image_search = false;
    draw = false;

    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
    ui->about->setEnabled(false);
    ui->about->setVisible(false);
    ui->status->setGeometry(width(),-100,10,10);
    ui->verticalSlider->setGeometry(width()-width()/10,100,width()/10,height()-200);

    filter_list.append("*.jpg");
    filter_list.append("*.png");

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


    repaint();
    }



}

void Widget::paintEvent(QPaintEvent *ev) // отрисовка
{
    QTime time;
    time.start();

    QPainter p(this);
    QPen pen;
    QBrush brush;
    QFont font;

    /*QPixmap mp(":/new/prefix1/Bez_imeni1.png");
    qDebug() << mp.size();
    p.drawPixmap(0,0,mp);*/


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
        int br_size_x = level->get_brick_size_x();
        int br_size_y = level->get_brick_size_y();
        QRect rect;
        pen.setColor(Qt::gray);
        p.setPen(pen);
        for (int i = 0; i < level->get_map_size(); i++)
        {
            rect.setTopLeft(QPoint(level->get_brick_coord(i).x(),level->get_brick_coord(i).y()));
            rect.setWidth(br_size_x);
            rect.setHeight(br_size_y);
            p.fillRect(rect,QColor(level->get_brick_color(i))); // рисуем кирпичик
            p.drawRect(rect); // рисуем контур
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

        ui->status->setGeometry(width()-(width()-level->get_grid_x()*br_size_x),
                                0,width()-level->get_grid_x()*br_size_x,height());
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


    if (dir_search && draw)
    {
        int shift = ui->verticalSlider->value()*(width()/5+60)/50;
        pen.setColor(Qt::white);
        p.setPen(pen);
        font.setPixelSize(20);
        p.setFont(font);
        QRect rect;
        for (int i = 0; i < pixmap_array.size(); i++)
        {
            rect.setTopLeft(QPoint(10+(i%4)*(width()/5+20),120+i/4*(width()/5+60)-shift));
            rect.setHeight(width()/5);
            rect.setWidth(width()/5);

            p.drawPixmap(rect,pixmap_array.at(i));
            QString str = dirs_with_img.at(i).right(dirs_with_img.at(i).size()-1 - dirs_with_img.at(i).lastIndexOf("/"));
            p.drawText(10 + (i%4)*(width()/5+20),90+i/4*(width()/5+60)-shift,width()/5,width()/5,0,str);
        }
        for (int i = 0; i < dirs_with_img.size(); i++)
        {
            rect.setTopLeft(QPoint(10+(i%4)*(width()/5+20),120+i/4*(width()/5+60)-shift));
            rect.setHeight(width()/5);
            rect.setWidth(width()/5);
            p.drawRect(rect);
        }

        p.fillRect(0,0,width(),80,Qt::black);
        font.setPixelSize(40);
        p.setFont(font);
        p.drawText(10,50,"Выберите папку:");
        //draw = false;
    }

    if (image_search && draw)
    {
        int shift = ui->verticalSlider->value()*(width()/6+5)/50;
        pen.setColor(Qt::white);
        p.setPen(pen);
        font.setPixelSize(20);
        p.setFont(font);
        QRect rect;
        for (int i = 0; i < pixmap_array.size(); i++)
        {
            rect.setTopLeft(QPoint((i%5)*(width()/6+5),120+i/5*(width()/6+5)-shift));
            rect.setHeight(width()/6);
            rect.setWidth(width()/6);

            p.drawPixmap(rect,pixmap_array.at(i));

        }
        for (int i = 0; i < images.size(); i++)
        {
            rect.setTopLeft(QPoint((i%5)*(width()/6+5),120+i/5*(width()/6+5)-shift));
            rect.setHeight(width()/6);
            rect.setWidth(width()/6);
            p.drawRect(rect);

        }
        p.fillRect(0,0,width(),80,Qt::black);
        font.setPixelSize(40);
        p.setFont(font);
        p.drawText(10,50,"Выберите картинку для игры:");
        //draw = false;
    }
    qDebug() << time.elapsed();

}

void Widget::menu_change_state(bool state)
{
    // свернуть/показать меню
    ui->start_button->setEnabled(state);
    ui->exit_button->setEnabled(state);
    ui->about_button->setEnabled(state);
    ui->settings_button->setEnabled(state);
    ui->status->setEnabled(!state);
    ui->verticalSlider->setEnabled(!state);

    ui->start_button->setVisible(state);
    ui->exit_button->setVisible(state);
    ui->about_button->setVisible(state);
    ui->settings_button->setVisible(state);
    ui->status->setVisible(!state);
    ui->verticalSlider->setVisible(!state);

    repaint();
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

    ui->verticalSlider->setValue(0);
    ui->verticalSlider->setGeometry(width()-width()/10,50,width()/10,height()-100);

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
    dirs_with_img.clear();

    QTime time1;
    time1.start();

    dir_search = true;
    #ifdef Q_OS_WIN // кроссплатформенность
    find_all_img("C:/");
    #else
    find_all_img("/sdcard");
    find_all_img("/storage/extSdCard");
    #endif


    draw = true;
    repaint();

    //qDebug() << dirs_with_img;

    qDebug() << time1.elapsed();
}

void Widget::mousePressEvent(QMouseEvent *m)
{
    if (game_running && !pause)
    {
        if (m->x() - level->get_board_width()/2 < 0)
            level->set_board_coord(level->get_board_width(), m->y()-100);
        else if (m->x() + level->get_board_width()/2 > width()-ui->status->width())
            level->set_board_coord(width()-ui->status->width() - level->get_board_width(), m->y()-100);
        else
            level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);
        if (m->y()-100 > height()-level->get_board_height())
            level->set_board_coord(level->get_board_x(),height()-level->get_board_height());

    }



    if (dir_search)
    {
        int shift = ui->verticalSlider->value()*(width()/5+60)/50;
        for (int i = 0; i < dirs_with_img.size(); i++)
        {
            if (m->x() >= 10+(i%4)*(width()/5+20)
                    && m->x() <= 10+(i%4)*(width()/5+20) + width()/5
                    && m->y() >= 120+i/4*(width()/5+60) - shift
                    && m->y() <= 120+i/4*(width()/5+60) + width()/5 - shift)
            {
                dir_search = false;
                image_search = true;
                qDebug() << "pressed" << i;

                ui->verticalSlider->setValue(0);

                find_all_img(dirs_with_img.at(i));

                on_verticalSlider_valueChanged(0);

                repaint();
            }

        }
    }
    else if (image_search)
    {
        int shift = ui->verticalSlider->value()*(width()/6+5)/50;
        for (int i = 0; i < images.size(); i++)
        {
            if (m->x() >= (i%5)*(width()/6+5)
                    && m->x() <= (i%5)*(width()/6+5) + width()/6
                    && m->y() >= 120+i/5*(width()/6+5) - shift
                    && m->y() <= 120+i/5*(width()/6+5) + width()/6 - shift)
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
                ui->verticalSlider->setEnabled(false);
                ui->verticalSlider->setVisible(false);
            }

        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *m)
{
    if (game_running && !pause)
    {
        if (m->x() - level->get_board_width()/2 < 0)
            level->set_board_coord(0, m->y()-100);
        else if (m->x() + level->get_board_width()/2 > width()-ui->status->width())
            level->set_board_coord(width()-ui->status->width() - level->get_board_width(), m->y()-100);
        else
            level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-100);

        if (m->y()-100 > height()-level->get_board_height())
            level->set_board_coord(level->get_board_x(),height()-level->get_board_height());

    }
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
#ifdef Q_OS_WIN // кроссплатформенность
    if (k->key() == Qt::Key_Backspace)
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
            pixmap_array.clear();
            images.clear();
            on_start_button_clicked();
        }

    }
#else
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
#endif

    draw = true;
    repaint();
}



void Widget::find_all_img(QString start_dir)
{
    //!
    //! \brief dir
    //!
    //! D:/ с фильтрами и перебором на . и .. - (3903 - 4036)
    //! D:/ без фильтров и перебором на . и .. - (6066 - 6182)
    //! D:/ с фильтрами без перебора на . и .. - (3880 - 3953)
    //! D:/ с фильтрами на . и .. - (3912 - 3962)
    //! D:/ с фильтрами и вынесенным фильтром - (1552 - 1564)
    //!
    //!
    //! C:/ с фильтрами с перебором на . и .. - (302811)
    //!
    //!


    dir.cd(start_dir);
    QStringList dir_list = dir.entryList(QDir::Dirs | QDir::NoSymLinks);
    QStringList files_list = dir.entryList(filter_list, QDir::Files | QDir::NoSymLinks, QDir::Time);

    if (dir_search)
    {
        if (files_list.size() != 0)
        {
            qDebug() << start_dir << files_list.size();
            dirs_with_img.append(start_dir);
            pixmap_array.append(QPixmap(start_dir+"/"+files_list.first()).scaled(width()/5,width()/5));
        }

        for (int i = 0; i < dir_list.size(); i++)
        {
            if (start_dir == "C:/")
            {
                if (dir_list.at(i) == "Users")
                    find_all_img(start_dir + "/" + dir_list.at(i));
                else
                    continue;
            }
            else
            {
                if (dir_list.at(i).startsWith("."))
                    continue;
                else if (dir_list.at(i) == "cache"
                        || dir_list.at(i) == "data"
                        || dir_list.at(i) == "docs"
                        || dir_list.at(i) == "lib"
                        || dir_list.at(i) == "help"
                        || dir_list.at(i) == "build"
                        || dir_list.at(i) == "plugin"
                        || dir_list.at(i) == "features"
                        || dir_list.at(i) == "plugins")
                    continue;
                find_all_img(start_dir + "/" + dir_list.at(i));
            }
        }

        ui->verticalSlider->setMaximum(dirs_with_img.size()/4*50);
    }

    if (image_search)
    {
        pixmap_array.clear();
        images.clear();
        for (int i = 0; i < files_list.size(); i++)
        {
            images.append(QString(start_dir+"/"+files_list.at(i)));
            //pixmap_array.append(QPixmap(images.last()));
            qDebug() << "added: " << files_list.at(i);
        }

        ui->verticalSlider->setMaximum(images.size()/5*50);
        draw = true;
        repaint();
    }
}


void Widget::on_verticalSlider_valueChanged(int value)
{

    if (image_search)
    {
        for (int i = pixmap_array.size(); i < (value+350)/50*6; i++)
        {
            if (i >= pixmap_array.size() && i < images.size())
            {
                pixmap_array.append(QPixmap(images.at(i)).scaled(width()/6,width()/6));
                repaint();
            }
        }
    }
    repaint();
}
