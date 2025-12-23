#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include "structs.h"
#include "generic_manager.h"
#include <iomanip>
#include <sstream>

using namespace std;

class PipeManager : public GenericManager<Pipe> {
public:
    PipeManager(int& id, Logger& log) : GenericManager<Pipe>(id, log) {}

    // Поиск свободной трубы по диаметру
    int FindFreePipeID(int diameter) {
        for (const auto& pipe : items) {
            // Труба свободна, если она никуда не подключена (ids == 0) и подходит по диаметру
            if (pipe.diametr == diameter && pipe.source_cs_id == 0 && pipe.dest_cs_id == 0) {
                return pipe.id;
            }
        }
        return -1; // Не найдено
    }

    // Привязка трубы к станциям
    void LinkPipe(int pipeId, int sourceId, int destId) {
        Pipe* p = FindById(pipeId);
        if (p) {
            p->source_cs_id = sourceId;
            p->dest_cs_id = destId;
        }
    }
    
    // Отвязка трубы (удаление из сети)
    void UnlinkPipe(int pipeId) {
        Pipe* p = FindById(pipeId);
        if (p) {
            p->source_cs_id = 0;
            p->dest_cs_id = 0;
        }
    }

private:
    void OnAdd(const Pipe& pipe) override {
        stringstream ss;
        ss << "ADDED PIPE - ID: " << pipe.id << ", Diam: " << pipe.diametr;
        logger.Log(ss.str());
    }

    void OnDelete(const Pipe& pipe) override {
        stringstream ss;
        ss << "DELETED PIPE - ID: " << pipe.id;
        logger.Log(ss.str());
    }
};

#endif