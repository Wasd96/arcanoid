#ifndef CORE_H
#define CORE_H

#include "level.h"

class Core
{
private:
    Level *level;

    int current_level;

public:
    Core();

    void start_game(int w, int h);

    Level* get_level() { return level; }
};

#endif // CORE_H
