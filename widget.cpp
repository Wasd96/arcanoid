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
    bonus_width = 0;
    bonus_explosive = 0;
    bonus_super_ball = 0;
    game_running = false;
    game_over = false;
    game_win = false;
    pause = false;
    dir_search = false;
    image_search = false;
    settings = false;
    first_time = false;
    opacity_color.setAlpha(0);

    ui->menu->setVisible(false);
    ui->menu->setEnabled(false);
    ui->about->setEnabled(false);
    ui->about->setVisible(false);
    ui->refresh_cache->setVisible(false);
    ui->refresh_cache->setEnabled(false);
    ui->status->setGeometry(width(),-100,10,10);
    ui->verticalSlider->setGeometry(width()-width()/10,100,width()/10,height()-200);
    ui->refresh_cache->setGeometry(width()/2 - ui->refresh_cache->width()/2,
                                   height()/7*2, width()/2, height()/10);

    filter_list.append("*.jpg");
    filter_list.append("*.png");



    QFile file("cache.dat");
    if (file.size() == 0)
    {
        if(file.open(QIODevice::WriteOnly))
        {
            qDebug() << "File Has Been Created / Already exist";
        }
        else
            qDebug() << "Failed to Create File";
    }
    file.close();
    file.open(QIODevice::ReadOnly);

    if (file.size() == 0)
        first_time = true;

    QString string;
    QPixmap pixmap;
    QString time;
    if (file.isOpen())
    {
        QDataStream stream(&file);
        do
        {
            stream >> string >> time >> pixmap;
            if (string != "")
            {
                cache_dirs_with_img.append(string);
                cache_img_time.append(time);
                cache_pixmap_array.append(pixmap);
            }
            qDebug() << string << time;
        } while(string != "");
    qDebug() << file.fileName();
    file.close();
    }

#ifdef Q_OS_WIN
    setMouseTracking(true);
    setWindowIcon(QIcon(":/new/prefix1/icon.png"));
    setWindowTitle("Image Destroyer");
#endif

    image_brick = new QImage(5,5,QImage::Format_RGB16);


    if (first_time)
        on_about_button_clicked();

}

Widget::~Widget()
{
    delete ui;
    delete level;
    //delete img;
}

void Widget::timerEvent(QTimerEvent *t)
{
    if (t->timerId() == bonus_width)
    {
        killTimer(bonus_width);
        level->set_board_width(level->get_board_width()/2);
        bonus_width = 0;
    }

    if (t->timerId() == bonus_explosive)
    {
        killTimer(bonus_explosive);
        level->set_explosive(false);
        bonus_explosive = 0;
    }

    if (t->timerId() == bonus_super_ball)
    {
        killTimer(bonus_super_ball);
        level->set_super_ball(false);
        bonus_super_ball = 0;
    }


    if (t->timerId() == timer)
    {
        if (game_running && !pause)
        {
            int state = level->update(width(), height(), image_brick); // состояние игрового мира
            /*if (state > -2)
                qDebug() << state;*/
            if (state == -1) // проигрыш
            {
                game_over = true;
                game_running = false;
                level->set_board_width(level->get_board_width()/2);
                level->set_super_ball(false);
                level->set_explosive(false);
            }
            else if (state == 0) // победа
            {
                ui->status->setValue(ui->status->maximum() - state);
                game_win = true;
                game_running = false;
                level->set_board_width(level->get_board_width()/2);
                level->set_super_ball(false);
                level->set_explosive(false);
            }
            else if (state > 0)
            {
                ui->status->setValue(ui->status->maximum() - state);

                int item = rand()%50; // бонусы

                if (bonus_explosive == 0 && bonus_super_ball == 0
                        && rand()%(level->get_ball_counter()+1)==0)
                {
                    if (item%5 == 0)
                    {
                        if (bonus_width == 0)
                        {
                            bonus_width = startTimer(20000);
                            level->set_board_width(level->get_board_width()*2);
                            opacity_text = "Широкая доска";
                            opacity_color.setRgb(50,200,50,255);
                        }
                    }
                    else if (item%8 == 0 )
                    {
                        if (level->get_ball_counter() <= 3)
                        {
                            level->double_ball();
                            opacity_text = "Удвоение";
                            opacity_color.setRgb(50,255,50,255);
                        }

                    }
                    else if (item%13 == 0)
                    {

                        {
                            bonus_explosive = startTimer(8000);
                            level->set_explosive(true);
                            level->set_super_ball(false);
                            opacity_text = "Взрывной";
                            opacity_color.setRgb(255,50,50,255);
                        }
                    }
                    else if (item%42 == 0)
                    {

                        {
                            bonus_super_ball = startTimer(4000);
                            level->set_explosive(false);
                            level->set_super_ball(true);
                            opacity_text = "Пробивной";
                            opacity_color.setRgb(250,50,150,255);
                        }
                    }
                }
            }


        repaint();
        }

        else if (opacity_color.alpha() > 0)
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

#ifdef Q_OS_WIN
    if (game_running && !pause)
    {
        QCursor curs;
        curs.setShape(Qt::BlankCursor);
        setCursor(curs);
    }
    else
    {
        QCursor curs;
        curs.setShape(Qt::ArrowCursor);
        setCursor(curs);
    }
#endif

    if (game_running || game_over || game_win)
    {
        int br_size_x = level->get_brick_size_x();

        // отрисовка уровня
        p.drawImage(0,0,*image_brick);

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
        if (!game_win)
        {
            if (level->get_explosive())
                pen.setColor(Qt::red);
            else if (level->get_super_ball())
                pen.setColor(qRgb(250,50,150));
            else
                pen.setColor(Qt::green);
            pen.setWidth(5);
            p.setPen(pen);
            for (int i = 0; i < level->get_ball_counter(); i++)
            {
                p.drawEllipse(level->get_ball_x(i), level->get_ball_y(i), 5, 5);
            }

            ui->status->setGeometry(width()-(width()-level->get_grid_x()*br_size_x),
                                    0,width()-level->get_grid_x()*br_size_x,height());
        }

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

    if (opacity_color.alpha() > 0)
    {
        pen.setColor(opacity_color);
        p.setPen(pen);
        font.setPixelSize(width()/20);
        p.setFont(font);
        p.drawText(width()/2-opacity_text.length()*font.pixelSize()/4, height()/5*3+50, opacity_text);
        if (opacity_color.alpha() >= 1)
            opacity_color.setAlpha(opacity_color.alpha()-1);
    }

    if (dir_search)
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
    }

    if (image_search)
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

    }

    if (settings)
    {
        pen.setColor(Qt::white);
        p.setPen(pen);
        font.setPixelSize(20);
        p.setFont(font);
        QRect rect;
        rect.setCoords(20, 40, width()-20, height());
#ifdef Q_OS_WIN // кроссплатформенность
        QString str("На наличие картинок "
                    "сканируется только папка C:/Users/ ради экономии "
                    "времени и пропуска системных картинок. Если Вы хотите "
                    "сыграть в картинку, лежащую в другом месте, то скопируйте "
                    "ее на рабочий стол или в любую папку внутри C:/Users/"
                    "\n\nКэш нужен для быстрой загрузки без лишнего поиска. Если вы добавили новую папку, "
                    "то необходимо нажать на кнопку ниже."
                    "\n\nСтоит учесть, что пропускаются папки с названиями cache, docs, lib, "
                    "plugin, build, data, help, src. Зачастую в этих папках содержатся ненужные "
                    "или неинтересные картинки.");
#else
        QString str("На наличие картинок сканируются память телефона и карты памяти. "
                    "\n\nКэш нужен для быстрой загрузки без лишнего поиска. Если вы добавили новую папку, "
                    "то необходимо нажать на кнопку ниже."
                    "\n\nСтоит учесть, что пропускаются папки с названиями cache, docs, lib, "
                    "plugin, build, data, help, src. Зачастую в этих папках содержатся ненужные "
                    "или неинтересные картинки.");
#endif

        p.drawText(rect, str);

        str = "Приятной игры!";
        font.setPixelSize(40);
        p.setFont(font);
        pen.setColor(QColor(92,154,204));
        p.setPen(pen);
        p.drawText(width()/2-str.length()*font.pixelSize()/4, height()/9*7, str);
    }


    //qDebug() << time.elapsed();

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

    QDir dir;
    QFileInfoList info_list;
    if (cache_dirs_with_img.size() != 0)
    {
        for (int i = 0; i < cache_dirs_with_img.size(); i++)
        {
            dir.cd(cache_dirs_with_img.at(i));
            info_list = dir.entryInfoList(filter_list, QDir::Files | QDir::NoSymLinks, QDir::Time);
            QString str;
            str = info_list.first().lastModified().toString();
            if (str == cache_img_time.at(i))
            {
                dirs_with_img.append(cache_dirs_with_img.at(i));
                pixmap_array.append(cache_pixmap_array.at(i));
            }
            else
            {
                dirs_with_img.append(cache_dirs_with_img.at(i));
                pixmap_array.append(info_list.first().filePath());
            }
        }
        ui->verticalSlider->setMaximum(dirs_with_img.size()/4*50);
    }
    int status = 0;
    #ifdef Q_OS_WIN // кроссплатформенность
    status = find_all_img("C:/",0);
    #else
    status = find_all_img("/sdcard",0);
    status = find_all_img("/storage/extSdCard",0);
    #endif

    if (status == 0)
    {
        cache_img_time.clear();
        cache_dirs_with_img = dirs_with_img;
        cache_pixmap_array = pixmap_array;
        QString str;

        for (int i = 0; i < cache_dirs_with_img.size(); i++)
        {
            dir.cd(cache_dirs_with_img.at(i));
            info_list = dir.entryInfoList(filter_list, QDir::Files | QDir::NoSymLinks, QDir::Time);
            str = info_list.first().lastModified().toString();
            cache_img_time.append(str);
        }

        QFile file("cache.dat");
        file.open(QIODevice::WriteOnly);
        if (file.isOpen())
        {
            QDataStream stream(&file);
            for (int i = 0; i< cache_dirs_with_img.size(); i++)
            {
                stream << cache_dirs_with_img.at(i) << cache_img_time.at(i) << cache_pixmap_array.at(i);
            }
            qDebug() << "writed";
        file.close();
        }
    }


    repaint();

    //qDebug() << dirs_with_img;

   qDebug() << time1.elapsed();
}

void Widget::mousePressEvent(QMouseEvent *m)
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

                find_all_img(dirs_with_img.at(i),1);

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
                QImage *temp_img = new QImage(level->get_grid_x()*level->get_brick_size_x()+1,
                                              level->get_grid_y()*level->get_brick_size_y()+1,
                                              QImage::Format_RGB16);
                image_brick = temp_img;
                level->update_img(image_brick);
                level->set_ball_coord(width()/2-level->get_board_width()/2, height()/5*4 - 5, 0);
                level->set_ball_angle(55+rand()%70, 0);
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
#ifdef Q_OS_WIN // кроссплатформенность
int add_y = 0;
#else
int add_y = 100;
#endif
    if (game_running && !pause)
    {
        if (m->x() - level->get_board_width()/2 < 0)
            level->set_board_coord(0, m->y()-add_y);
        else if (m->x() + level->get_board_width()/2 > width()-ui->status->width())
            level->set_board_coord(width()-ui->status->width() - level->get_board_width(), m->y()-add_y);
        else
            level->set_board_coord(m->x()-level->get_board_width()/2, m->y()-add_y);

        if (m->y()-add_y > height()-level->get_board_height())
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
    ui->verticalSlider->setVisible(false);
    ui->verticalSlider->setEnabled(false);
    ui->refresh_cache->setVisible(false);
    ui->refresh_cache->setEnabled(false);
    game_over = false;
    game_win = false;
    game_running = false;
    pause = false;
    settings = false;
    menu_change_state(true);
}

void Widget::on_about_button_clicked()
{
    menu_change_state(false);
    ui->status->setVisible(false);
    ui->verticalSlider->setVisible(false);
    ui->verticalSlider->setEnabled(false);
    ui->refresh_cache->setVisible(false);
    ui->refresh_cache->setEnabled(false);
    ui->about->setEnabled(true);
    ui->about->setVisible(true);
    ui->about->setGeometry(width()/20, height()/20, width()/20*18, height()/20*14);
    ui->about->setText("\tДобро пожаловать!\n\n"
                       "Перед Вами игра по типу \"Арканоид\"."
                       "\nОна создана ради пробы своих сил "
                       "и просто удовольствия.\n"
                       "Особенность заключается в том, "
                       "что игроку не нужно проходить заготовленные одинаковые уровни - для игры нужно "
                       "просто выбрать любую картинку. (Прозрачность в *.png учитывается)\n\n"
                       "Код игры писал я, Волков Александр, но она бы не увидела свет без некоторых людей.\n"
                       "Объявляю благодарности:\n"
                       "Дмитрию С. - за идею;\n"
                       "Полине О. - за помощь;\n"
                       "Андрею О. - за поддержку.\n\n"
                       "При большом желании со мной можно связаться в вк - vk.com/just_another_member , "
                       "или пишите на e-mail - wasd3680@yandex.ru\n\nРепозиторий проекта на гитхабе "
                       "доступен всем желающим и может быть найден по адресу https://github.com/Wasd96/arcanoid");

    ui->menu->setGeometry(0, height()-height()/10, width()/4, height()/10);
    ui->menu->setVisible(true);
    ui->menu->setEnabled(true);
}

void Widget::on_settings_button_clicked()
{
    menu_change_state(false);
    settings = true;
    ui->status->setVisible(false);
    ui->verticalSlider->setVisible(false);
    ui->verticalSlider->setEnabled(false);
    ui->refresh_cache->setVisible(true);
    ui->refresh_cache->setEnabled(true);
    ui->refresh_cache->setGeometry(width()/2 - ui->refresh_cache->width()/2,
                                   height()/7*4, width()/2, height()/10);



    ui->menu->setGeometry(0, height()-height()/10, width()/4, height()/10);
    ui->menu->setVisible(true);
    ui->menu->setEnabled(true);
    repaint();
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

    if (pause)
    {
        ui->menu->setGeometry(0, height()-height()/10, width()/4, height()/10);
        ui->menu->setVisible(true);
        ui->menu->setEnabled(true);
    }
    if (!pause && game_running)
    {
        ui->menu->setVisible(false);
        ui->menu->setEnabled(false);
    }
    repaint();
}



int Widget::find_all_img(QString start_dir, bool check)
{
    dir.cd(start_dir);
    QStringList dir_list = dir.entryList(QDir::Dirs | QDir::NoSymLinks);
    QStringList files_list = dir.entryList(filter_list, QDir::Files | QDir::NoSymLinks, QDir::Time);


    if (!check)
    {
        if (cache_pixmap_array.size() != 0)
            return 1;
    }



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
                    find_all_img(start_dir + "/" + dir_list.at(i), 1);
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
                        || dir_list.at(i).contains("build")
                        || dir_list.at(i).contains("plugin")
                        || dir_list.at(i) == "features"
                        || dir_list.at(i) == "src")
                    continue;
                find_all_img(start_dir + "/" + dir_list.at(i), 1);
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
        }

        ui->verticalSlider->setMaximum(images.size()/5*50);
        repaint();
    }
    return 0;
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

void Widget::on_refresh_cache_clicked()
{
    qDebug() << "cache updating...";
    cache_dirs_with_img.clear();
    cache_pixmap_array.clear();
    cache_img_time.clear();
    pixmap_array.clear();
    dirs_with_img.clear();
    dir_search = true;
    #ifdef Q_OS_WIN // кроссплатформенность
    find_all_img("C:/",1);
    #else
    find_all_img("/sdcard",1);
    find_all_img("/storage/extSdCard",1);
    #endif


    cache_dirs_with_img = dirs_with_img;
    cache_pixmap_array = pixmap_array;
    QString str;
    QDir dir;
    QFileInfoList info_list;

    for (int i = 0; i < cache_dirs_with_img.size(); i++)
    {
        dir.cd(cache_dirs_with_img.at(i));
        info_list = dir.entryInfoList(filter_list, QDir::Files | QDir::NoSymLinks, QDir::Time);
        str = info_list.first().lastModified().toString();
        cache_img_time.append(str);
    }

    QFile file("cache.dat");
    file.open(QIODevice::WriteOnly);
    if (file.isOpen())
    {
        QDataStream stream(&file);
        for (int i = 0; i< cache_dirs_with_img.size(); i++)
        {
            stream << cache_dirs_with_img.at(i) << cache_img_time.at(i) << cache_pixmap_array.at(i);
        }
        qDebug() << "rewrited";
    file.close();
    }
    dir_search = false;
    repaint();
}
