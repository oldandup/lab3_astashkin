#ifndef COMPRESS_MANAGER_H
#define COMPRESS_MANAGER_H

#include "structs.h"
#include "generic_manager.h"
#include <sstream>

using namespace std;

class CompressManager : public GenericManager<Compress> {
public:
    CompressManager(int& id, Logger& log) : GenericManager<Compress>(id, log) {}

private:
    void OnAdd(const Compress& station) override {
        stringstream ss;
        ss << "ADDED CS - ID: " << station.id << ", Name: " << station.name 
           << ", Workshops: " << station.workshop_count 
           << ", Working: " << station.workshop_working
           << ", Class: " << station.classification 
           << ", Active: " << (station.working ? "Yes" : "No");
        logger.Log(ss.str());
    }

    void OnDelete(const Compress& station) override {
        stringstream ss;
        ss << "DELETED CS - ID: " << station.id << ", Name: " << station.name 
           << ", Workshops: " << station.workshop_count 
           << ", Working: " << station.workshop_working;
        logger.Log(ss.str());
    }
};

#endif
