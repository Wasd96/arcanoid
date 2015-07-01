#include "level.h"

Level::Level()
{
    map.append(Brick(QPoint(0,0),QColor(0,0,0)));
}

Level::Level(short x, short y)
{
    grid_x = x;
    grid_y = y;
    map.append(Brick(QPoint(0,0),QColor(0,0,0)));
}

void Level::set_brick_size(int w, int h)
{
    brick_size_x = w / grid_x;
    brick_size_y = h / grid_y;
}

void Level::set_grid(int x, int y)
{
    grid_x = x;
    grid_y = y;
}

void Level::load_map(int curr_level)
{
    /*
    map = new Brick*[grid_x];
       for (int i = 0; i < grid_x; i++)
           map[i] = new Brick[grid_y];


       for (int i=0; i<grid_x; i++)
           for (int j=0; j<grid_y; j++) {
               Brick brick = new Brick;
               file >> sector;
               map[i][j] = sector;
           }
    */

    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++) {
            Brick *brick = new Brick(QPoint(i*10, j*10),
                                    QColor(i*100%255, j*100%255, (i+j)*100%255));
            map.push_front(*brick);
            delete brick;
        }

}
