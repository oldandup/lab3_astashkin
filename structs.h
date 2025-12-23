#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

using namespace std;

struct Pipe {
    int id;
    string km_mark;           // километровая отметка
    double length;            // длина в км
    int diametr;              // диаметр в мм
    bool repair;              // признак "в ремонте"
    
    // Новые поля для графа
    int source_cs_id = 0;     // ID станции "откуда" (0 - не подключена)
    int dest_cs_id = 0;       // ID станции "куда" (0 - не подключена)
};

struct Compress {
    int id;
    string name;              // название
    int workshop_count;       // количество цехов
    int workshop_working;     // количество цехов в работе
    string classification;    // класс станции
    bool working;             // статус работы
};

#endif