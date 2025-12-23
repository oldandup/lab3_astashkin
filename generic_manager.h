#ifndef GENERIC_MANAGER_H
#define GENERIC_MANAGER_H

#include "logger.h"
#include <vector>

using namespace std;

template<typename T>
class GenericManager {
protected:
    vector<T> items;
    int& nextId;
    Logger& logger;

public:
    GenericManager(int& id, Logger& log) : nextId(id), logger(log) {}

    virtual ~GenericManager() = default;

    void Add(const T& item) {
        T newItem = item;
        newItem.id = nextId++;
        items.push_back(newItem);
        OnAdd(newItem);
    }

    T* FindById(int id) {
        for (auto& item : items) {
            if (item.id == id) return &item;
        }
        return nullptr;
    }

    bool Delete(int id) {
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i].id == id) {
                OnDelete(items[i]);
                items.erase(items.begin() + i);
                return true;
            }
        }
        return false;
    }

    vector<T>& GetAll() { return items; }
    const vector<T>& GetAll() const { return items; }
    void Clear() { items.clear(); }

protected:
    virtual void OnAdd(const T& item) = 0;
    virtual void OnDelete(const T& item) = 0;
};

#endif
